#include "ECS/Components/World/TileMapComp.h"

namespace Spoon {
  void TileMapComp::OnReflect() 
  {
	ImGui::Text("Tileset ID: %s", m_Atlas.textureId.c_str());

    if (ImGui::BeginChild(
            "Tileset Explorer",
            ImVec2(0, 200),
            ImGuiChildFlags_Borders))
    {
        for (const auto& [id, texture] :
             ResourceManager::Get().GetTextures())
        {
            if (ImGui::ImageButton(
                    id.c_str(),
                    texture,
                    sf::Vector2f(64, 64)))
            {
                m_Atlas.textureId = id;
				m_Atlas.Resolve();
            }

            ImGui::SameLine();
        }

        ImGui::EndChild();
    }

    ImGui::InputInt("Atlas Margin", &m_Atlas.margin);
    ImGui::InputInt("Atlas Spacing", &m_Atlas.spacing);
  }

  void TileMapComp::PreRender(EntityManager& manager, UUID id)
  {
      // dont know if we need anything here yet - leaving no-op for now
  }

  void TileMapComp::Render(sf::RenderTarget& target, sf::RenderStates states)
  {
        for (std::size_t i = 0; i < m_Layers.size(); ++i)
		{
			if (!m_Atlas.texture) continue;
			
			if (!m_Layers[i].visible) continue;
	
			const auto& vertices = m_LayerVertices[i];
			if (vertices.empty()) continue;
	
			sf::RenderStates layerStates = states;
			layerStates.texture = *m_Atlas.texture;
	
			target.draw(vertices.data(), vertices.size(),
						sf::PrimitiveType::Triangles, layerStates);
		}
  }

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
                    sf::Vector2f{static_cast<float>(x * m_Atlas.tileWidth), static_cast<float>(y * m_Atlas.tileHeight)},
                    sf::Vector2f{static_cast<float>(m_Atlas.tileWidth), static_cast<float>(m_Atlas.tileHeight)}
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

                vertices.emplace_back(sf::Vertex{topLeft, color, texTopLeft});
                vertices.emplace_back(sf::Vertex{topRight, color, texTopRight});
                vertices.emplace_back(sf::Vertex{bottomRight, color, texBottomRight});

                vertices.emplace_back(sf::Vertex{topLeft, color, texTopLeft});
                vertices.emplace_back(sf::Vertex{bottomRight, color, texBottomRight});
                vertices.emplace_back(sf::Vertex{bottomLeft, color, texBottomLeft});
            }
        }
    }
  }

  sf::IntRect TileMapComp::GetAtlasRect(uint16_t id) const
  {
	  if ( id == 0 || 
		  id > m_Atlas.columns * m_Atlas.rows ||
		  m_Atlas.columns <= 0 || 
		  m_Atlas.rows <= 0 || 
		  m_Atlas.tileWidth <= 0 || 
		  m_Atlas.tileHeight <= 0)
		  return sf::IntRect{
			  {0, 0},
			  {0, 0}
		  };
	  
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

  sf::Color TileMapComp::LayerColor(const TileLayer& layer) const
  {
      const float opacity = std::clamp(layer.opacity, 0.0f, 1.0f);

      return sf::Color{
          255,
          255,
          255,
          static_cast<std::uint8_t>(opacity * 255.0f)
      };
  }

bool TileMapComp::SetTile(uint16_t id, int x, int y, int layerIndex)
{
	if (layerIndex < 0 || static_cast<std::size_t>(layerIndex) >= m_Layers.size())
        return false;

    if (x < 0 || y < 0 || x >= m_MapSize.x || y >= m_MapSize.y)
        return false;

    if (m_MapSize.x <= 0 || m_MapSize.y <= 0)
        return false;

    auto& layer = m_Layers[static_cast<std::size_t>(layerIndex)];
    const std::size_t width = static_cast<std::size_t>(m_MapSize.x);
    const std::size_t height = static_cast<std::size_t>(m_MapSize.y);

    const std::size_t expectedCount = width * height;

    if (layer.tiles.size() != expectedCount)
        layer.tiles.resize(expectedCount, Tile{ 0 });

    const std::size_t index =
        static_cast<std::size_t>(y) * width +
        static_cast<std::size_t>(x);

    layer.tiles[index].id = id;

    BuildMap();
    return true;
}

bool TileMapComp::ClearTile(int x, int y, int layerIndex)
{
    return SetTile(0, x, y, layerIndex);
}

  void TileAtlas::Resolve() { texture = ResourceManager::Get().GetResource<sf::Texture>(textureId); }
}
