#pragma once

#include "ECS/Components/Component.h"
#include "Core/Renderer/Renderable.h"
#include "Core/Renderer/WorldProjection.h"
#include "Utils/Graphics/GridSlicer.h"

namespace Spoon {

  // base tile
  struct Tile {
    int id;                     // index to rect in the atlas
    bool collidable = false;    // collision extraction flag
  };
  
  // atlas reference - we'll probably config this so change the defaults 
  // depending on the asset provided for the project
  struct TileAtlas {
    std::string textureId;   				// atlas texture id - fetch from resource manager on resolve()
    sf::Texture* texture = nullptr; // runtime texture ptr - load on resolve()
    
    AtlasGrid grid;                                 
    int tileFootprintX = 0;         // projected (isometric diamond) width of a cell - 0 uses tileWidth
    int tileFootprintY = 0;         // projected (isometric diamond) height of a cell - 0 uses tileHeight
    
	  bool fetchBadTexture = false;  	// editor flag

    void Resolve();                 // get texture in memory from resource manager
  };
    
  // abstraction for the tilemap comp to hold. we'll build the vertex array from this
  struct TileLayer {
    std::string name;              // "Background", "Gameplay", "Foreground"
    bool visible = true;           // runtime/editor visibility
    bool collidable = false;       // collision extraction
    float opacity = 1.0f;          // editor/runtime blending
    std::vector<Tile> tiles;       // row-major: width * height
  };

  struct TileMapComp : public ComponentBase<TileMapComp>, public IRenderable {
      public:
          TileMapComp() : ComponentBase::ComponentBase(Name) {}
      
          static constexpr const char* Name = "TileMap";

          void OnReflect() override;
          void OnKill(EntityManager* manager) override;

          void ClampInput();
          bool ValidateBounds(int x, int y, int layerIndex) const;

          // tile/layer methods
          bool SetTile(int id, int x, int y, int layerIndex);
          bool ClearTile(int x, int y, int layerIndex);
          bool FillLayer(int layerIndex, int tileId);
          bool ClearLayer(int layerIndex);

		      std::optional<int> GetTile(int x, int y, int layerIndex) const;
          std::vector<Tile> GetLayerTiles(int layerIndex) const;

          // helpers for deriving projection state
          sf::FloatRect GetTileBounds(int x, int y, int layerIndex) const;  
          sf::Vector2f GetLogicalCellSize() const;                          
          sf::Vector2f GetCellSize() const;                                 
          WorldProjection GetProjection(bool projectionEnabled) const;
          sf::FloatRect GetTileDrawRect(int x, int y, const WorldProjection& projection) const;

          // build the map
          void BuildMap();
		      sf::Color LayerColor(const TileLayer& layer) const;

          // members
          sf::Vector2i m_MapSize;      // map size in width/height cells, not px
          TileAtlas m_Atlas;
          std::vector<TileLayer> m_Layers;
          std::vector<std::vector<sf::Vertex>> m_LayerVertices;
          std::vector<UUID> m_ColliderEntities;

          // rebuild flags
          bool m_NeedsRebuild = true;
          bool m_RebuildCollision = true;
          bool m_BuiltProjected = false;  // projection gate the render cache was built with

          // renderable interface
          void PreRender(EntityManager& manager, UUID id) override;
          void Render(sf::RenderTarget& target, sf::RenderStates states) override;
          sf::Vector2f GetPosition() override { return sf::Vector2f{0.0, 0.0}; }
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tile, id, collidable)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TileAtlas, textureId, grid, tileFootprintX, tileFootprintY)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileLayer, name, visible, collidable, opacity, tiles)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileMapComp, m_MapSize, m_Atlas, m_Layers)
}
