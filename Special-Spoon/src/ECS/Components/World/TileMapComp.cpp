#include "ECS/Components/World/TileMapComp.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "Core/Application.h"
#include "Utils/Macros.h"

namespace Spoon {

	void TileMapComp::OnKill(EntityManager* manager)
	{
		for (UUID id : m_ColliderEntities)
		{
			manager->ReapEntity(id);
		}
	}

	void TileMapComp::ClampInput()
	{
		if (m_Atlas.tileWidth < 0) m_Atlas.tileWidth = 0;
		if (m_Atlas.tileHeight < 0) m_Atlas.tileHeight = 0;
		if (m_Atlas.tileFootprintX < 0) m_Atlas.tileFootprintX = 0;
		if (m_Atlas.tileFootprintY < 0) m_Atlas.tileFootprintY = 0;
		if (m_Atlas.columns < 0) m_Atlas.columns = 0;
		if (m_Atlas.rows < 0) m_Atlas.rows = 0;
	    if (m_Atlas.margin < 0) m_Atlas.margin = 0;
	    if (m_Atlas.spacing < 0) m_Atlas.spacing = 0;
	}

  void TileMapComp::OnReflect() 
  {
  	constexpr const char* missingAtlasPopup = "Missing Tilemap Atlas";
	if (m_Atlas.fetchBadTexture)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
        ImGui::TextWrapped("The tilemap atlas texture could not be found.");
        ImGui::PopStyleColor();

        if (ImGui::Button("Show Details")) ImGui::OpenPopup(missingAtlasPopup);
        ImGui::SameLine();

        if (ImGui::Button("Clear Atlas"))
        {
            m_Atlas.textureId.clear();
            m_Atlas.texture = nullptr;
            m_Atlas.fetchBadTexture = false;
            m_NeedsRebuild = true;
        }
    }

    if (ImGui::BeginPopupModal(missingAtlasPopup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped("The texture referenced by this tilemap is unavailable.");

        ImGui::Spacing();

        ImGui::TextWrapped("Referenced texture:");
        ImGui::Indent();
        ImGui::TextWrapped("%s", m_Atlas.textureId.empty() ? "<empty>" : m_Atlas.textureId.c_str());
        ImGui::Unindent();

        ImGui::Spacing();

        ImGui::TextWrapped(
            "Select a valid texture from the explorer below. "
            "The tilemap will remain empty until the atlas is resolved."
        );

        ImGui::Spacing();

        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::TextDisabled("Map Settings");
    ImGui::Separator();

	// map size controls
	bool mapSizeChanged = false;
    mapSizeChanged |= ImGui::InputInt("Map Width", &m_MapSize.x);
    mapSizeChanged |= ImGui::InputInt("Map Height", &m_MapSize.y);
    if (mapSizeChanged) 
	{
		m_MapSize.x = std::max(0, m_MapSize.x);
		m_MapSize.y = std::max(0, m_MapSize.y);
		m_NeedsRebuild = true;
        m_RebuildCollision = true;
    }
    
	ImGui::TextDisabled("Atlas Settings"); 
	ImGui::Separator();
	  
	// texture browser
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
				if (!m_Atlas.fetchBadTexture) 
					m_NeedsRebuild = true;
            }

            ImGui::SameLine();
        }

        ImGui::EndChild();
    }

    if (ImGui::Button("Open Tile Map Editor"))
        Application::Get().GetEditor().EditTileMap(this);

	// atlas settings controls
	bool changed = false;
	changed |= ImGui::InputInt("Tile Width", &m_Atlas.tileWidth); ImGui::SameLine();
	HelpMarker("The width of a single tile in the atlas texture.");
	changed |= ImGui::InputInt("Tile Height", &m_Atlas.tileHeight); ImGui::SameLine();
	HelpMarker("The height of a single tile in the atlas texture.");
	changed |= ImGui::InputInt("Tile Footprint X", &m_Atlas.tileFootprintX); ImGui::SameLine();
	HelpMarker("The horizontal footprint of a single tile in the atlas texture.");
	changed |= ImGui::InputInt("Tile Footprint Y", &m_Atlas.tileFootprintY); ImGui::SameLine();
	HelpMarker("The vertical footprint of a single tile in the atlas texture.");
	changed |= ImGui::InputInt("Columns", &m_Atlas.columns); ImGui::SameLine();
	HelpMarker("The number of columns in the atlas texture.");
	changed |= ImGui::InputInt("Rows", &m_Atlas.rows); ImGui::SameLine();
	HelpMarker("The number of rows in the atlas texture.");
    changed |= ImGui::InputInt("Atlas Margin", &m_Atlas.margin); ImGui::SameLine();
    HelpMarker("The margin around the atlas texture. Follows a {1, 1} line from the top-left corner. If your first tile starts at px (1, 1), set this to 1.");
    changed |= ImGui::InputInt("Atlas Spacing", &m_Atlas.spacing); ImGui::SameLine();
    HelpMarker("The horizontal spacing between tiles in the same row of the atlas texture.");

	if (changed) {
		ClampInput();
		m_NeedsRebuild = true;
		m_RebuildCollision = true;
	}
  }

  void TileMapComp::PreRender(EntityManager& manager, UUID id)
  {
      // rebuild the render cache when the project's projection gate changes
	  const bool projected = Application::Get().GetRenderer().IsProjectionEnabled();
	  if (projected != m_BuiltProjected)
	  {
		m_BuiltProjected = projected;
		m_NeedsRebuild = true;
	  }

	  if (m_NeedsRebuild)
	  {
		BuildMap();
		m_NeedsRebuild = false;
	  }
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
			layerStates.texture = m_Atlas.texture;
	
			target.draw(vertices.data(), vertices.size(),
						sf::PrimitiveType::Triangles, layerStates);
		}
  }

  // BuildMap() only produces the render cache (presentation space). Tile
  // collision is built separately from the logical tile grid by
  // TileMapCollisionSystem and never reads these vertices.
  void TileMapComp::BuildMap() {
    m_LayerVertices.clear();
    m_LayerVertices.resize(m_Layers.size());

    if (m_MapSize.x <= 0 || m_MapSize.y <= 0)
        return;

    if (m_Atlas.tileWidth <= 0 || m_Atlas.tileHeight <= 0)
        return;

    const WorldProjection projection = GetProjection(m_BuiltProjected);

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

                const sf::FloatRect worldRect =
                    GetTileDrawRect(x, y, projection);

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

	bool TileMapComp::ValidateBounds(int x, int y, int layerIndex) const 
	{
		if (layerIndex < 0 || static_cast<std::size_t>(layerIndex) >= m_Layers.size())
	        return false;
	
	    if (x < 0 || y < 0 || x >= m_MapSize.x || y >= m_MapSize.y)
	        return false;

		return true;
	}

	bool TileMapComp::SetTile(uint16_t id, int x, int y, int layerIndex)
	{
		if (id != 0 &&
		    (m_Atlas.columns <= 0 ||
		     m_Atlas.rows <= 0 ||
		     id > m_Atlas.columns * m_Atlas.rows))
		{
		    return false;
		}
		
	    if (m_MapSize.x <= 0 || m_MapSize.y <= 0)
	        return false;
		if (!ValidateBounds(x, y, layerIndex))
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
	
	    m_NeedsRebuild = true;
		m_RebuildCollision = true;
	    return true;
	}

	bool TileMapComp::ClearTile(int x, int y, int layerIndex)
	{
	    return SetTile(0, x, y, layerIndex);
	}

	bool TileMapComp::FillLayer(int layerIndex, uint16_t tileId)
	{
		if (tileId != 0 &&
		    (m_Atlas.columns <= 0 ||
		     m_Atlas.rows <= 0 ||
		     tileId > m_Atlas.columns * m_Atlas.rows))
		{
		    return false;
		}
		
		if (layerIndex < 0 || static_cast<std::size_t>(layerIndex) >= m_Layers.size())
			return false;
		auto& layer = m_Layers[static_cast<std::size_t>(layerIndex)];

		if (m_MapSize.x < 0 || m_MapSize.y < 0)
			return false;
		const std::size_t expectedCount =
		    static_cast<std::size_t>(m_MapSize.x) *
		    static_cast<std::size_t>(m_MapSize.y);
		layer.tiles.resize(expectedCount, Tile{0});
		
		for (auto& tile : layer.tiles) 
			tile.id = tileId;
		m_NeedsRebuild = true;
		m_RebuildCollision = true;
		return true;
	}

	bool TileMapComp::ClearLayer(int layerIndex)
	{
		if (layerIndex < 0 || static_cast<std::size_t>(layerIndex) >= m_Layers.size())
			return false;
		auto& layer = m_Layers[static_cast<std::size_t>(layerIndex)];

		if (m_MapSize.x < 0 || m_MapSize.y < 0)
			return false;
		const std::size_t expectedCount =
		    static_cast<std::size_t>(m_MapSize.x) *
		    static_cast<std::size_t>(m_MapSize.y);
		layer.tiles.resize(expectedCount, Tile{0});
		
		for (auto& tile : layer.tiles) 
			tile.id = 0;
		m_NeedsRebuild = true;
		m_RebuildCollision = true;
		return true;
	}

	std::optional<uint16_t> TileMapComp::GetTile(int x, int y, int layerIndex) const
	{
		if (!ValidateBounds(x, y, layerIndex))
			return std::nullopt;
		
		std::size_t index = static_cast<std::size_t>(layerIndex);
		auto& layer = m_Layers[index];
		const std::size_t width = static_cast<std::size_t>(m_MapSize.x);
		const std::size_t tileIndex =
	        static_cast<std::size_t>(y) * width +
	        static_cast<std::size_t>(x);
		
		const std::size_t expectedCount =
		    static_cast<std::size_t>(m_MapSize.x) *
		    static_cast<std::size_t>(m_MapSize.y);
	
		if (layer.tiles.size() != expectedCount ||
		    tileIndex >= layer.tiles.size())
		{
		    return std::nullopt;
		}
		
		return layer.tiles[tileIndex].id;
	}

	std::vector<Tile> TileMapComp::GetLayerTiles(int layerIndex) const
	{
		if (layerIndex < 0 || static_cast<std::size_t>(layerIndex) >= m_Layers.size())
			return {};
		auto& layer = m_Layers[static_cast<std::size_t>(layerIndex)];
		return layer.tiles;
	}

	sf::FloatRect TileMapComp::GetTileBounds(int x, int y, int layerIndex) const
	{
		if (!ValidateBounds(x, y, layerIndex))
			return sf::FloatRect{};

		// logical space - matches the tile collision cache
		const sf::Vector2f cellSize = GetLogicalCellSize();
		return sf::FloatRect{
			sf::Vector2f{static_cast<float>(x) * cellSize.x, static_cast<float>(y) * cellSize.y},
			cellSize
		};
	}

	sf::Vector2f TileMapComp::GetLogicalCellSize() const
	{
		return {
			static_cast<float>(m_Atlas.tileWidth),
			static_cast<float>(m_Atlas.tileHeight)
		};
	}

	sf::Vector2f TileMapComp::GetCellSize() const
	{
		return {
			static_cast<float>(m_Atlas.tileFootprintX > 0 ? m_Atlas.tileFootprintX : m_Atlas.tileWidth),
			static_cast<float>(m_Atlas.tileFootprintY > 0 ? m_Atlas.tileFootprintY : m_Atlas.tileHeight)
		};
	}

	WorldProjection TileMapComp::GetProjection(bool projectionEnabled) const
	{
		WorldProjection projection;
		projection.enabled = projectionEnabled;
		projection.logicalCell = GetLogicalCellSize();
		projection.projectedCell = GetCellSize();
		return projection;
	}

	sf::FloatRect TileMapComp::GetTileDrawRect(int x, int y, const WorldProjection& projection) const
	{
		// the atlas tile is drawn at its full atlas size. when projected, its
		// top-center is anchored to the top corner of the cell's diamond so the
		// tile's top face covers the cell footprint.
		const sf::Vector2f tileSize{
			static_cast<float>(m_Atlas.tileWidth),
			static_cast<float>(m_Atlas.tileHeight)
		};
		const sf::Vector2f anchor = projection.CellToScreen({
			static_cast<float>(x),
			static_cast<float>(y)
		});

		if (!projection.IsActive())
			return sf::FloatRect{anchor, tileSize};

		return sf::FloatRect{
			sf::Vector2f{anchor.x - tileSize.x * 0.5f, anchor.y},
			tileSize
		};
	}
		
	void TileAtlas::Resolve() 
	{ 
		fetchBadTexture = false;
		texture = nullptr;
		if (!textureId.empty())
		{
			try {
				texture = &ResourceManager::Get().GetResource<sf::Texture>(textureId);
			} catch (const std::out_of_range&) {
				fetchBadTexture = true;
				SS_DEBUG_LOG("[TILEMAP] Failed to resolve atlas texture: " + textureId);
			}
		}
	}
}
