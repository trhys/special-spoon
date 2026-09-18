#pragma once

#include "ECS/Components/Component.h"
#include "Core/Renderer/Renderable.h"

#include <optional>

namespace Spoon {

  // base tile
  struct Tile {
    uint16_t id;    // index to rect in the atlas
  };

  // atlas reference - we'll probably config this so change the defaults 
  // depending on the asset provided for the project
  struct TileAtlas {
    std::string textureId;   						// atlas texture id - fetch from resource manager on resolve()
    sf::Texture* texture = nullptr;                 // runtime texture ptr - load on resolve()
    int tileWidth = 16;
    int tileHeight = 16;
    int columns = 1;                                // derived or serialized
    int rows = 1;                                   // derived or serialized
    int margin = 0;                                 // atlas spacing support
    int spacing = 0;

    void Resolve();                                 // get texture in memory from resource manager
  };
    
  // abstraction for the tilemap comp to hold. we'll build the vertex array from this
  struct TileLayer {
    std::string name;              // "Background", "Gameplay", "Foreground"
    bool visible = true;           // runtime/editor visibility
    bool collidable = false;       // optional: collision extraction
    float opacity = 1.0f;          // editor/runtime blending
    std::vector<Tile> tiles;       // row-major: width * height
	bool fetchBadTexture = false;  // editor flag
  };

  struct TileMapComp : public ComponentBase<TileMapComp>, public IRenderable {
      public:
          TileMapComp() : ComponentBase::ComponentBase(Name) {}
      
          static constexpr const char* Name = "TileMap";

          void OnReflect() override;
		  void ClampInput();
		  bool ValidateBounds(int x, int y, int layerIndex);

          // tile/layer methods
          bool SetTile(uint16_t id, int x, int y, int layerIndex);
          bool ClearTile(int x, int y, int layerIndex);
		  bool FillLayer(int layerIndex, uint16_t tileId);
		  bool ClearLayer(int layerIndex);

		  std::optional<uint16_t> GetTile(int x, int y, int layerIndex) const;

          // build the map
          void BuildMap();
          sf::IntRect GetAtlasRect(uint16_t id) const;
		  sf::Color LayerColor(const TileLayer& layer) const;

          // members
          sf::Vector2i m_MapSize;      // map size in width/height cells, not px
          TileAtlas m_Atlas;
          std::vector<TileLayer> m_Layers;
          std::vector<std::vector<sf::Vertex>> m_LayerVertices;

		  // renderable interface
		  void PreRender(EntityManager& manager, UUID id) override;
		  void Render(sf::RenderTarget& target, sf::RenderStates states) override;
		  sf::Vector2f GetPosition() override { return sf::Vector2f{0.0, 0.0}; }
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tile, id)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileAtlas, textureId, tileWidth, tileHeight, columns, rows, margin, spacing)  
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileLayer, name, visible, collidable, opacity, tiles)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileMapComp, m_MapSize, m_Atlas, m_Layers)
}
