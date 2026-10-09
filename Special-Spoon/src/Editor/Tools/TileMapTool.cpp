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

        // the tool works on its own tilemap's grid, gated by the project config
        WorldProjection ToolProjection(const TileMapComp& tileMap)
        {
            return tileMap.GetProjection(
                Application::Get().GetRenderer().IsProjectionEnabled()
            );
        }

        ImVec2 WorldToImage(
            Viewport& viewport,
            const ImVec2& imageMin,
            const sf::Vector2f& world
        )
        {
            const sf::Vector2i pixel = viewport.target.mapCoordsToPixel(world);
            return ImVec2(
                imageMin.x + static_cast<float>(pixel.x),
                imageMin.y + static_cast<float>(pixel.y)
            );
        }
    }

    void TileMapTool::Open(TileMapComp* tileMap)
    {
        m_SceneGeneration = Application::Get().GetSceneManager().GetSceneGeneration();
        m_TileMap = tileMap;
        m_Open = (m_TileMap != nullptr);
        m_HasPendingBuild = false;
        m_LastPaintedCell = {-1, -1};
        m_LastLeftDown = false;
        m_LastRightDown = false;

        if (m_TileMap)
            EnsureValidState(*m_TileMap);
    }

    void TileMapTool::FlushPendingBuild()
    {
        if (!m_HasPendingBuild)
            return;

        m_TileMap->BuildMap();
        m_TileMap->m_RebuildCollision = true;

        m_HasPendingBuild = false;
    }

    void TileMapTool::Close()
    {
        FlushPendingBuild();
        m_Open = false;
        m_TileMap = nullptr;
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

        /*m_TileMap = tileMap;*/
        return true;
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
        tileMap.m_RebuildCollision = true;
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
                tileMap.m_RebuildCollision = true;

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
            tileMap.m_RebuildCollision = true;
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
            tileMap.m_RebuildCollision = true;
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
        const sf::Vector2f imageSize(
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

            const bool projected = ToolProjection(tileMap).IsActive();
            const sf::Vector2f cell = tileMap.GetCellSize();

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
                const ImU32 outlineColor = isSelected
                    ? IM_COL32(255, 255, 0, 255)
                    : IM_COL32(255, 255, 255, 100);
                const float outlineThickness = isSelected ? 2.0f : 1.0f;

                const float localX = localClick.x - static_cast<float>(atlasRect.position.x);
                const float localY = localClick.y - static_cast<float>(atlasRect.position.y);
                bool insideSlot = false;

                if (projected)
                {
                    // project from the top face of the tile anchored
                    // to the top center
                    const float cx = (rectMin.x + rectMax.x) * 0.5f;
                    const float dHalfW = cell.x * 0.5f * m_PaletteScale;
                    const float dHalfH = cell.y * 0.5f * m_PaletteScale;
                    const ImVec2 top   (cx,          rectMin.y);
                    const ImVec2 right (cx + dHalfW, rectMin.y + dHalfH);
                    const ImVec2 bottom(cx,          rectMin.y + dHalfH * 2.0f);
                    const ImVec2 left  (cx - dHalfW, rectMin.y + dHalfH);
                    ImVec2 diamond[4] = { top, right, bottom, left };

                    drawList->AddPolyline(
                        diamond,
                        4,
                        outlineColor,
                        ImDrawFlags_Closed,
                        outlineThickness
                    );

                    // Diamond hit test in normalized local slot space.
                    const float halfW = cell.x * 0.5f;
                    const float halfH = cell.y * 0.5f;
                    const float centerX = static_cast<float>(atlasRect.size.x) * 0.5f;
                    insideSlot =
                        halfW > 0.0f &&
                        halfH > 0.0f &&
                        (std::abs((localX - centerX) / halfW) + std::abs((localY - halfH) / halfH) <= 1.0f);
                }
                else
                {
                    drawList->AddRect(
                        rectMin,
                        rectMax,
                        outlineColor,
                        0.0f,
                        0,
                        outlineThickness
                    );

                    insideSlot =
                        localX >= 0.0f &&
                        localX < static_cast<float>(atlasRect.size.x) &&
                        localY >= 0.0f &&
                        localY < static_cast<float>(atlasRect.size.y);
                }

                if (clicked && insideSlot)
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

        // presentation -> cell (inverse projection when gated)
        const sf::Vector2f cell = ToolProjection(*m_TileMap).ScreenToCell(worldMouse);
        cellX = static_cast<int>(std::floor(cell.x));
        cellY = static_cast<int>(std::floor(cell.y));

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

        // cell corners -> presentation. a diamond when projected, the
        // logical cell rect otherwise.
        const WorldProjection projection = ToolProjection(*m_TileMap);
        auto corner = [&](int x, int y) {
            return WorldToImage(
                viewport,
                imageMin,
                projection.CellToScreen({ static_cast<float>(x), static_cast<float>(y) })
            );
        };

        ImVec2 poly[4] = {
            corner(cellX,     cellY),
            corner(cellX + 1, cellY),
            corner(cellX + 1, cellY + 1),
            corner(cellX,     cellY + 1)
        };

        const bool erasePreview =
            m_EraseMode || m_SelectedTileId == 0 ||
            ImGui::IsMouseDown(ImGuiMouseButton_Right);
        const ImU32 fillColor = erasePreview
            ? IM_COL32(220, 80, 80, 80)
            : IM_COL32(255, 220, 80, 80);
        const ImU32 outlineColor = erasePreview
            ? IM_COL32(255, 100, 100, 255)
            : IM_COL32(255, 230, 120, 255);

        auto* drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRect(imageMin, imageMax, true);
        drawList->AddConvexPolyFilled(poly, 4, fillColor);
        drawList->AddPolyline(poly, 4, outlineColor, true, 2.0f);
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

        const WorldProjection projection = ToolProjection(*m_TileMap);
        const int columns = m_TileMap->m_MapSize.x;
        const int rows = m_TileMap->m_MapSize.y;

        if (projection.logicalCell.x > 0.0f &&
            projection.logicalCell.y > 0.0f &&
            columns > 0 && rows > 0)
        {
            auto projectToScreen = [&](float x, float y) -> ImVec2 {
                return WorldToImage(viewport, imageMin, projection.CellToScreen({ x, y }));
            };

            auto* drawList = ImGui::GetWindowDrawList();
            const ImU32 color = IM_COL32(255, 255, 255, 65);
            drawList->PushClipRect(imageMin, imageMax, true);

            // Constant-column lines, including both map edges.
            for (int x = 0; x <= columns; ++x)
            {
                drawList->AddLine(
                    projectToScreen(static_cast<float>(x), 0.0f),
                    projectToScreen(static_cast<float>(x), static_cast<float>(rows)),
                    color
                );
            }

            // Constant-row lines, including both map edges.
            for (int y = 0; y <= rows; ++y)
            {
                drawList->AddLine(
                    projectToScreen(0.0f, static_cast<float>(y)),
                    projectToScreen(static_cast<float>(columns), static_cast<float>(y)),
                    color
                );
            }

            drawList->PopClipRect();
        }

        const bool leftDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        const bool rightDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);

        if (!viewportHovered)
        {
            if (!leftDown && !rightDown)
                FlushPendingBuild();

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
            FlushPendingBuild();
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
