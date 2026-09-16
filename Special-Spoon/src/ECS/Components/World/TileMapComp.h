#pragma once

#include "ECS/Components/Component.h"

namespace Spoon {

  // base tile
  struct Tile {
    uint16_t id;    // index to rect in the atlas
  };

  // atlas reference - we'll probably config this so change the defaults 
  // depending on the asset provided for the project
  struct TileAtlas {
    std::string texturePath;         // atlas texture path - fetch from resource manager on resolve()
    sf::Texture* texture = nullptr;  // runtime texture ptr - load on resolve()
    int tileWidth = 16;
    int tileHeight = 16;
    int columns = 1;                 // derived or serialized
    int rows = 1;                    // derived or serialized
    int margin = 0;                  // atlas spacing support
    int spacing = 0;

    void Resolve();                  // get texture in memory from resource manager
  };
    
  // abstraction for the tilemap comp to hold. we'll build the vertex array from this
  struct TileLayer {
    std::string name;              // "Background", "Gameplay", "Foreground"
    int zOrder = 0;                // deterministic layer sort
    bool visible = true;           // runtime/editor visibility
    bool collidable = false;       // optional: collision extraction
    float opacity = 1.0f;          // editor/runtime blending
    std::vector<Tile> tiles;       // row-major: width * height
  };

  struct TileMapComp : public ComponentBase<TileMapComp> {
      public:
          TileMapComp() : ComponentBase::ComponentBase(Name) { m_Atlas.Resolve(); }
      
          static constexpr const char* Name = "TileMap";

          void OnReflect() override;

          // set and clear tiles in the map
          void SetTile(uint16_t id, int x, int y, int layerIndex);
          void ClearTile(int x, int y, int layerIndex);

          // build the map
          void BuildMap();

      private:
          sf::Vector2i m_MapSize;      // map size in width/height cells, not px
          TileAtlas m_Atlas;
          std::vector<TileLayer> m_Layers;
          std::vector<std::vector<sf::Vertex>> m_LayerVertices;
  };
}
