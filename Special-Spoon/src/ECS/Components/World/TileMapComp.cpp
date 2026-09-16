#include "ECS/Components/World/TileMapComp.h"

namespace Spoon {
  void TileMapComp::BuildMap() {
    m_LayerVertices.clear();
    m_LayerVertices.resize(m_Layers.size());

    if (m_MapSize.x <= 0 || m_MapSize.y <= 0)
        return;

    if (m_Atlas.tileWidth <= 0 || m_Atlas.tileHeight <= 0)
        return;

    for (std::size_t layerIndex = 0; layerIndex < m_Layers.size(); ++layerIndex)
    {
        const TileLayer& layer = m_Layers[layerIndex];
        auto& vertices = m_LayerVertices[layerIndex];

        vertices.clear();

        const std::size_t expectedTileCount =
            static_cast<std::size_t>(m_MapSize.x) *
            static_cast<std::size_t>(m_MapSize.y);

        const std::size_t tileCount =
            std::min(layer.tiles.size(), expectedTileCount);

        // Six vertices per tile: two triangles.
        vertices.reserve(tileCount * 6);

        for (int y = 0; y < m_MapSize.y; ++y)
        {
            for (int x = 0; x < m_MapSize.x; ++x)
            {
                const std::size_t tileIndex =
                    static_cast<std::size_t>(y) *
                    static_cast<std::size_t>(m_MapSize.x) +
                    static_cast<std::size_t>(x);

                if (tileIndex >= tileCount)
                    continue;

                const Tile& tile = layer.tiles[tileIndex];

                // Reserve tile ID 0 for an empty cell.
                if (tile.id == 0)
                    continue;

                const sf::FloatRect worldRect{
                    static_cast<float>(x * m_Atlas.tileWidth),
                    static_cast<float>(y * m_Atlas.tileHeight),
                    static_cast<float>(m_Atlas.tileWidth),
                    static_cast<float>(m_Atlas.tileHeight)
                };

                const sf::IntRect atlasRect =
                    GetAtlasRect(tile.id);

                const sf::Vector2f topLeft{
                    worldRect.position.x,
                    worldRect.position.y
                };

                const sf::Vector2f topRight{
                    worldRect.position.x + worldRect.size.x,
                    worldRect.position.y
                };

                const sf::Vector2f bottomRight{
                    worldRect.position.x + worldRect.size.x,
                    worldRect.position.y + worldRect.size.y
                };

                const sf::Vector2f bottomLeft{
                    worldRect.position.x,
                    worldRect.position.y + worldRect.size.y
                };

                const sf::Vector2f texTopLeft{
                    static_cast<float>(atlasRect.position.x),
                    static_cast<float>(atlasRect.position.y)
                };

                const sf::Vector2f texTopRight{
                    static_cast<float>(
                        atlasRect.position.x + atlasRect.size.x),
                    static_cast<float>(atlasRect.position.y)
                };

                const sf::Vector2f texBottomRight{
                    static_cast<float>(
                        atlasRect.position.x + atlasRect.size.x),
                    static_cast<float>(
                        atlasRect.position.y + atlasRect.size.y)
                };

                const sf::Vector2f texBottomLeft{
                    static_cast<float>(atlasRect.position.x),
                    static_cast<float>(
                        atlasRect.position.y + atlasRect.size.y)
                };

                const sf::Color color = LayerColor(layer);

                vertices.emplace_back(topLeft, texTopLeft, color);
                vertices.emplace_back(topRight, texTopRight, color);
                vertices.emplace_back(bottomRight, texBottomRight, color);

                vertices.emplace_back(topLeft, texTopLeft, color);
                vertices.emplace_back(bottomRight, texBottomRight, color);
                vertices.emplace_back(bottomLeft, texBottomLeft, color);
            }
        }
    }
  }

  sf::IntRect TileMapComp::GetAtlasRect(uint16_t id) const
  {
      const int atlasColumn = static_cast<int>(id - 1) % m_Atlas.columns;
      const int atlasRow = static_cast<int>(id - 1) / m_Atlas.columns;
  
      const int x =
          m_Atlas.margin +
          atlasColumn * (m_Atlas.tileWidth + m_Atlas.spacing);
  
      const int y =
          m_Atlas.margin +
          atlasRow * (m_Atlas.tileHeight + m_Atlas.spacing);
  
      return sf::IntRect{
          {x, y},
          {m_Atlas.tileWidth, m_Atlas.tileHeight}
      };
  }
}
