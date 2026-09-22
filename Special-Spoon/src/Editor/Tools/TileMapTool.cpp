#include "TileMapTool.h"

#include "Core/Application.h"
#include "ECS/Components/World/TileMapComp.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace Spoon
{
    namespace
    {
        constexpr const char* addLayerPopup = "Add Tile Layer";

        TileMapComp* ResolveTileMap(UUID entity)
        {
            if (entity.ID == 0)
                return nullptr;

            auto& tileMaps =
                Application::Get()
                    .GetEntityManager()
                    .GetArray<TileMapComp>(TileMapComp::Name);

            const auto found = tileMaps.m_IdToIndex.find(entity);
            if (found == tileMaps.m_IdToIndex.end())
                return nullptr;

            return &tileMaps.m_Components[found->second];
        }
    }

    void TileMapTool::Open(UUID tileMapEntity)
    {
        m_TileMapEntity = tileMapEntity;
        m_SceneGeneration = Application::Get().GetSceneManager().GetSceneGeneration();
        m_TileMap = ResolveTileMap(m_TileMapEntity);
        m_Open = (m_TileMap != nullptr);
        m_HasPendingBuild = false;
        m_LastPaintedCell = {-1, -1};
        m_LastLeftDown = false;
        m_LastRightDown = false;

        if (m_TileMap)
            EnsureValidState(*m_TileMap);
    }

    void TileMapTool::Close()
    {
        m_Open = false;
        m_TileMapEntity = {};
        m_TileMap = nullptr;
        m_HasPendingBuild = false;
        m_LastPaintedCell = {-1, -1};
        m_LastLeftDown = false;
        m_LastRightDown = false;
    }

    bool TileMapTool::RefreshTileMap()
    {
        if (Application::Get().GetSceneManager().GetSceneGeneration() !=
            m_SceneGeneration)
        {
            return false;
        }

        m_TileMap = ResolveTileMap(m_TileMapEntity);
        return m_TileMap != nullptr;
    }

    void TileMapTool::EnsureValidState(TileMapComp& tileMap)
    {
        if (tileMap.m_Layers.empty())
        {
            TileLayer layer;
            layer.name = "Background";

            if (tileMap.m_MapSize.x > 0 && tileMap.m_MapSize.y > 0)
            {
                const std::size_t expectedCount =
                    static_cast<std::size_t>(tileMap.m_MapSize.x) *
                    static_cast<std::size_t>(tileMap.m_MapSize.y);
                layer.tiles.resize(expectedCount, Tile{0});
            }

            tileMap.m_Layers.push_back(std::move(layer));
            tileMap.BuildMap();
        }

        if (tileMap.m_Layers.empty())
            m_ActiveLayerIndex = 0;
        else
            m_ActiveLayerIndex = std::clamp(
                m_ActiveLayerIndex,
                0,
                static_cast<int>(tileMap.m_Layers.size()) - 1
            );

        const int tileCount = tileMap.m_Atlas.columns * tileMap.m_Atlas.rows;
        if (tileCount <= 0)
            m_SelectedTileId = 0;
        else if (m_SelectedTileId != 0)
            m_SelectedTileId = std::clamp(m_SelectedTileId, 1, tileCount);
    }

    void TileMapTool::ResizeLayerData(TileMapComp& tileMap, int layerIndex)
    {
        if (layerIndex < 0 ||
            static_cast<std::size_t>(layerIndex) >= tileMap.m_Layers.size() ||
            tileMap.m_MapSize.x <= 0 ||
            tileMap.m_MapSize.y <= 0)
        {
            return;
        }

        const std::size_t expectedCount =
            static_cast<std::size_t>(tileMap.m_MapSize.x) *
            static_cast<std::size_t>(tileMap.m_MapSize.y);

        tileMap.m_Layers[static_cast<std::size_t>(layerIndex)].tiles.resize(
            expectedCount,
            Tile{0}
        );
        tileMap.BuildMap();
    }

    void TileMapTool::DrawLayerPanel(TileMapComp& tileMap)
    {
        EnsureValidState(tileMap);

        const bool validMapSize =
            tileMap.m_MapSize.x > 0 && tileMap.m_MapSize.y > 0;
        const std::size_t expectedCount = validMapSize
            ? static_cast<std::size_t>(tileMap.m_MapSize.x) *
                  static_cast<std::size_t>(tileMap.m_MapSize.y)
            : 0;

        ImGui::Text("Layer count: %zu", tileMap.m_Layers.size());

        if (ImGui::Button("Add Layer"))
            ImGui::OpenPopup(addLayerPopup);

        if (ImGui::BeginPopupModal(
                addLayerPopup,
                nullptr,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText(
                "Layer Name",
                m_NewLayerName,
                IM_ARRAYSIZE(m_NewLayerName)
            );

            const bool hasName = m_NewLayerName[0] != '\0';
            ImGui::BeginDisabled(!hasName);
            if (ImGui::Button("Create"))
            {
                TileLayer layer;
                layer.name = m_NewLayerName;
                if (validMapSize)
                    layer.tiles.resize(expectedCount, Tile{0});

                tileMap.m_Layers.push_back(std::move(layer));
                m_ActiveLayerIndex =
                    static_cast<int>(tileMap.m_Layers.size()) - 1;
                tileMap.BuildMap();

                std::snprintf(
                    m_NewLayerName,
                    sizeof(m_NewLayerName),
                    "New Layer"
                );
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                std::snprintf(
                    m_NewLayerName,
                    sizeof(m_NewLayerName),
                    "New Layer"
                );
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginListBox("Layers"))
        {
            for (std::size_t index = 0; index < tileMap.m_Layers.size(); ++index)
            {
                const auto& layer = tileMap.m_Layers[index];
                const std::string displayName =
                    layer.name.empty() ? "Unnamed Layer" : layer.name;
                const bool selected =
                    m_ActiveLayerIndex == static_cast<int>(index);

                ImGui::PushID(static_cast<int>(index));
                if (ImGui::Selectable(displayName.c_str(), selected))
                    m_ActiveLayerIndex = static_cast<int>(index);
                ImGui::PopID();

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndListBox();
        }

        if (tileMap.m_Layers.empty())
            return;

        auto& activeLayer =
            tileMap.m_Layers[static_cast<std::size_t>(m_ActiveLayerIndex)];

        char layerName[128];
        std::snprintf(
            layerName,
            sizeof(layerName),
            "%s",
            activeLayer.name.c_str()
        );

        if (ImGui::InputText(
                "Active Layer Name",
                layerName,
                IM_ARRAYSIZE(layerName)))
        {
            activeLayer.name = layerName;
        }

        bool rebuildMap = false;
        rebuildMap |= ImGui::Checkbox("Visible", &activeLayer.visible);
        rebuildMap |= ImGui::Checkbox("Collidable", &activeLayer.collidable);
        rebuildMap |= ImGui::SliderFloat(
            "Opacity",
            &activeLayer.opacity,
            0.0f,
            1.0f,
            "%.2f"
        );

        if (rebuildMap)
        {
            activeLayer.opacity = std::clamp(activeLayer.opacity, 0.0f, 1.0f);
            tileMap.BuildMap();
        }

        const int tileDataCount = static_cast<int>(activeLayer.tiles.size());
        ImGui::Text(
            "Tile data: %d / %zu",
            tileDataCount,
            expectedCount
        );

        const bool sizeMatches = activeLayer.tiles.size() == expectedCount;
        if (!sizeMatches)
        {
            ImGui::TextDisabled(
                "Active layer tile data does not match the current map size."
            );

            ImGui::BeginDisabled(!validMapSize);
            if (ImGui::Button("Resize Active Layer"))
                ResizeLayerData(tileMap, m_ActiveLayerIndex);
            ImGui::EndDisabled();
        }

        ImGui::BeginDisabled(m_ActiveLayerIndex <= 0);
        if (ImGui::ArrowButton("Move Layer Up", ImGuiDir_Up))
        {
            std::swap(
                tileMap.m_Layers[static_cast<std::size_t>(m_ActiveLayerIndex)],
                tileMap.m_Layers[static_cast<std::size_t>(m_ActiveLayerIndex - 1)]
            );
            --m_ActiveLayerIndex;
            tileMap.BuildMap();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(
            m_ActiveLayerIndex >= static_cast<int>(tileMap.m_Layers.size()) - 1
        );
        if (ImGui::ArrowButton("Move Layer Down", ImGuiDir_Down))
        {
            std::swap(
                tileMap.m_Layers[static_cast<std::size_t>(m_ActiveLayerIndex)],
                tileMap.m_Layers[static_cast<std::size_t>(m_ActiveLayerIndex + 1)]
            );
            ++m_ActiveLayerIndex;
            tileMap.BuildMap();
        }
        ImGui::EndDisabled();

        if (ImGui::Button("Clear Active Layer"))
            tileMap.ClearLayer(m_ActiveLayerIndex);

        ImGui::BeginDisabled(tileMap.m_Layers.size() <= 1);
        if (ImGui::Button("Remove Active Layer"))
        {
            tileMap.m_Layers.erase(
                tileMap.m_Layers.begin() +
                static_cast<std::ptrdiff_t>(m_ActiveLayerIndex)
            );
            tileMap.BuildMap();
            EnsureValidState(tileMap);
        }
        ImGui::EndDisabled();
    }

    void TileMapTool::DrawTilePalette(TileMapComp& tileMap)
    {
        EnsureValidState(tileMap);

        ImGui::Text("Selected tile id: %d", m_SelectedTileId);

        if (ImGui::Button("Select Empty"))
        {
            m_SelectedTileId = 0;
            m_EraseMode = true;
        }

        ImGui::SameLine();
        ImGui::Checkbox("Erase Mode", &m_EraseMode);
        ImGui::SliderFloat("Palette Scale", &m_PaletteScale, 0.25f, 8.0f, "%.2fx");

        if (tileMap.m_Atlas.texture == nullptr ||
            tileMap.m_Atlas.tileWidth <= 0 ||
            tileMap.m_Atlas.tileHeight <= 0 ||
            tileMap.m_Atlas.columns <= 0 ||
            tileMap.m_Atlas.rows <= 0)
        {
            ImGui::BeginDisabled();
            ImGui::TextWrapped("Configure a valid tile atlas to use the palette.");
            ImGui::EndDisabled();
            return;
        }

        const sf::Vector2u atlasSize = tileMap.m_Atlas.texture->getSize();
        const ImVec2 imageSize(
            static_cast<float>(atlasSize.x) * m_PaletteScale,
            static_cast<float>(atlasSize.y) * m_PaletteScale
        );

        if (ImGui::BeginChild("Tile Palette", ImVec2(0, 320), ImGuiChildFlags_Borders))
        {
            ImGui::Image(*tileMap.m_Atlas.texture, imageSize);

            const ImVec2 imageMin = ImGui::GetItemRectMin();
            const ImVec2 imageMax = ImGui::GetItemRectMax();
            auto* drawList = ImGui::GetWindowDrawList();

            drawList->PushClipRect(imageMin, imageMax, true);

            int selectedFromClick = -1;
            const bool imageHovered = ImGui::IsItemHovered();
            const bool clicked =
                imageHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

            ImVec2 localClick(0.0f, 0.0f);
            if (clicked)
            {
                const ImVec2 mousePos = ImGui::GetIO().MousePos;
                localClick = ImVec2(
                    (mousePos.x - imageMin.x) / m_PaletteScale,
                    (mousePos.y - imageMin.y) / m_PaletteScale
                );
            }

            const int tileCount = tileMap.m_Atlas.columns * tileMap.m_Atlas.rows;
            for (int tileIndex = 1; tileIndex <= tileCount; ++tileIndex)
            {
                const sf::IntRect atlasRect =
                    tileMap.GetAtlasRect(static_cast<uint16_t>(tileIndex));

                const ImVec2 rectMin(
                    imageMin.x + atlasRect.position.x * m_PaletteScale,
                    imageMin.y + atlasRect.position.y * m_PaletteScale
                );
                const ImVec2 rectMax(
                    rectMin.x + atlasRect.size.x * m_PaletteScale,
                    rectMin.y + atlasRect.size.y * m_PaletteScale
                );

                const bool isSelected =
                    !m_EraseMode && m_SelectedTileId == tileIndex;

                drawList->AddRect(
                    rectMin,
                    rectMax,
                    isSelected ? IM_COL32(255, 255, 0, 255)
                               : IM_COL32(255, 255, 255, 100),
                    0.0f,
                    0,
                    isSelected ? 2.0f : 1.0f
                );

                if (clicked &&
                    localClick.x >= atlasRect.position.x &&
                    localClick.x < atlasRect.position.x + atlasRect.size.x &&
                    localClick.y >= atlasRect.position.y &&
                    localClick.y < atlasRect.position.y + atlasRect.size.y)
                {
                    selectedFromClick = tileIndex;
                }
            }

            drawList->PopClipRect();

            if (selectedFromClick > 0)
            {
                m_SelectedTileId = selectedFromClick;
                m_EraseMode = false;
            }
        }
        ImGui::EndChild();
    }

    bool TileMapTool::GetHoveredCell(
        Viewport& viewport,
        const ImVec2& imageMin,
        int& cellX,
        int& cellY
    ) const
    {
        if (!m_TileMap ||
            m_TileMap->m_Atlas.tileWidth <= 0 ||
            m_TileMap->m_Atlas.tileHeight <= 0 ||
            m_TileMap->m_MapSize.x <= 0 ||
            m_TileMap->m_MapSize.y <= 0)
        {
            return false;
        }

        const ImVec2 mousePos = ImGui::GetIO().MousePos;
        sf::Vector2i relativeMouse(
            static_cast<int>(mousePos.x - imageMin.x),
            static_cast<int>(mousePos.y - imageMin.y)
        );

        const sf::Vector2f worldMouse =
            viewport.target.mapPixelToCoords(relativeMouse);

        cellX = static_cast<int>(
            std::floor(worldMouse.x / static_cast<float>(m_TileMap->m_Atlas.tileWidth))
        );
        cellY = static_cast<int>(
            std::floor(worldMouse.y / static_cast<float>(m_TileMap->m_Atlas.tileHeight))
        );

        return m_TileMap->ValidateBounds(cellX, cellY, m_ActiveLayerIndex);
    }

    void TileMapTool::DrawCellOverlay(
        Viewport& viewport,
        const ImVec2& imageMin,
        const ImVec2& imageMax,
        int cellX,
        int cellY
    ) const
    {
        if (!m_TileMap)
            return;

        const sf::Vector2f worldMin(
            static_cast<float>(cellX * m_TileMap->m_Atlas.tileWidth),
            static_cast<float>(cellY * m_TileMap->m_Atlas.tileHeight)
        );
        const sf::Vector2f worldMax(
            worldMin.x + static_cast<float>(m_TileMap->m_Atlas.tileWidth),
            worldMin.y + static_cast<float>(m_TileMap->m_Atlas.tileHeight)
        );

        const sf::Vector2i pixelMin = viewport.target.mapCoordsToPixel(worldMin);
        const sf::Vector2i pixelMax = viewport.target.mapCoordsToPixel(worldMax);

        const ImVec2 rectMin(
            imageMin.x + static_cast<float>(pixelMin.x),
            imageMin.y + static_cast<float>(pixelMin.y)
        );
        const ImVec2 rectMax(
            imageMin.x + static_cast<float>(pixelMax.x),
            imageMin.y + static_cast<float>(pixelMax.y)
        );

        const ImU32 fillColor = (m_EraseMode || m_SelectedTileId == 0)
            ? IM_COL32(220, 80, 80, 80)
            : IM_COL32(255, 220, 80, 80);
        const ImU32 outlineColor = (m_EraseMode || m_SelectedTileId == 0)
            ? IM_COL32(255, 100, 100, 255)
            : IM_COL32(255, 230, 120, 255);

        auto* drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRect(imageMin, imageMax, true);
        drawList->AddRectFilled(rectMin, rectMax, fillColor);
        drawList->AddRect(rectMin, rectMax, outlineColor, 0.0f, 0, 2.0f);
        drawList->PopClipRect();
    }

    bool TileMapTool::HandleViewport(
        Viewport& viewport,
        bool viewportHovered,
        const ImVec2& imageMin,
        const ImVec2& imageMax
    )
    {
        if (!m_Open || !RefreshTileMap())
        {
            Close();
            return false;
        }

        const bool leftDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        const bool rightDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);

        if (!viewportHovered)
        {
            if (!leftDown && !rightDown && m_HasPendingBuild)
            {
                m_TileMap->BuildMap();
                m_HasPendingBuild = false;
            }

            m_LastPaintedCell = {-1, -1};
            return false;
        }

        EnsureValidState(*m_TileMap);

        if (leftDown != m_LastLeftDown || rightDown != m_LastRightDown)
            m_LastPaintedCell = {-1, -1};

        m_LastLeftDown = leftDown;
        m_LastRightDown = rightDown;

        int cellX = -1;
        int cellY = -1;
        if (GetHoveredCell(viewport, imageMin, cellX, cellY))
        {
            DrawCellOverlay(viewport, imageMin, imageMax, cellX, cellY);

            const sf::Vector2i hoveredCell(cellX, cellY);
            const bool changedCell = hoveredCell != m_LastPaintedCell;

            bool handledCell = false;
            if (leftDown && changedCell)
            {
                handledCell = true;
                auto& activeLayer = m_TileMap
                                        ->m_Layers[static_cast<std::size_t>(
                                            m_ActiveLayerIndex)];
                const std::size_t width =
                    static_cast<std::size_t>(m_TileMap->m_MapSize.x);
                const std::size_t expectedCount =
                    width * static_cast<std::size_t>(m_TileMap->m_MapSize.y);
                if (activeLayer.tiles.size() != expectedCount)
                    activeLayer.tiles.resize(expectedCount, Tile{0});

                const std::size_t tileIndex =
                    static_cast<std::size_t>(cellY) * width +
                    static_cast<std::size_t>(cellX);
                const std::uint16_t newTile = (m_EraseMode || m_SelectedTileId == 0)
                    ? static_cast<std::uint16_t>(0)
                    : static_cast<std::uint16_t>(m_SelectedTileId);
                if (activeLayer.tiles[tileIndex].id != newTile)
                {
                    activeLayer.tiles[tileIndex].id = newTile;
                    m_HasPendingBuild = true;
                }
            }
            else if (rightDown && changedCell)
            {
                handledCell = true;
                auto& activeLayer = m_TileMap
                                        ->m_Layers[static_cast<std::size_t>(
                                            m_ActiveLayerIndex)];
                const std::size_t width =
                    static_cast<std::size_t>(m_TileMap->m_MapSize.x);
                const std::size_t expectedCount =
                    width * static_cast<std::size_t>(m_TileMap->m_MapSize.y);
                if (activeLayer.tiles.size() != expectedCount)
                    activeLayer.tiles.resize(expectedCount, Tile{0});

                const std::size_t tileIndex =
                    static_cast<std::size_t>(cellY) * width +
                    static_cast<std::size_t>(cellX);
                if (activeLayer.tiles[tileIndex].id != 0)
                {
                    activeLayer.tiles[tileIndex].id = 0;
                    m_HasPendingBuild = true;
                }
            }

            if (handledCell)
                m_LastPaintedCell = hoveredCell;
        }

        if (!leftDown && !rightDown)
        {
            if (m_HasPendingBuild)
            {
                m_TileMap->BuildMap();
                m_HasPendingBuild = false;
            }
            m_LastPaintedCell = {-1, -1};
        }

        return true;
    }

    void TileMapTool::Update(sf::Time tick)
    {
        static_cast<void>(tick);

        if (!m_Open || !RefreshTileMap())
        {
            Close();
            return;
        }

        EnsureValidState(*m_TileMap);

        if (ImGui::Begin("Tile Map Tools", &m_Open))
        {
            DrawLayerPanel(*m_TileMap);
            ImGui::Separator();
            DrawTilePalette(*m_TileMap);
            ImGui::Separator();
            ImGui::TextDisabled("Viewport Controls");
            ImGui::BulletText("Left Mouse: Paint");
            ImGui::BulletText("Right Mouse: Erase");
            ImGui::BulletText("Middle Mouse: Pan");
            ImGui::BulletText("Mouse Wheel: Zoom");
        }
        ImGui::End();

        if (!m_Open)
            Close();
    }
}
