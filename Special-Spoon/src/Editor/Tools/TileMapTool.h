#pragma once

#include "ECS/UUID.h"
#include "Editor/Utils/Viewport.h"
#include "SFML/System/Time.hpp"

namespace Spoon
{
    struct TileMapComp;

    class TileMapTool
    {
    public:
        void Open(UUID tileMapEntity);
        void Close();

        bool IsOpen() const { return m_Open; }

        void Update(sf::Time tick);

        bool HandleViewport(
            Viewport& viewport,
            bool viewportHovered,
            const ImVec2& imageMin,
            const ImVec2& imageMax
        );

    private:
        bool RefreshTileMap();
        void DrawLayerPanel(TileMapComp& tileMap);
        void DrawTilePalette(TileMapComp& tileMap);

        void EnsureValidState(TileMapComp& tileMap);
        void ResizeLayerData(TileMapComp& tileMap, int layerIndex);

        bool GetHoveredCell(
            Viewport& viewport,
            const ImVec2& imageMin,
            int& cellX,
            int& cellY
        ) const;

        void DrawCellOverlay(
            Viewport& viewport,
            const ImVec2& imageMin,
            const ImVec2& imageMax,
            int cellX,
            int cellY
        ) const;

        bool m_Open = false;
        UUID m_TileMapEntity{};
        TileMapComp* m_TileMap = nullptr;

        int m_ActiveLayerIndex = 0;
        int m_SelectedTileId = 1;
        bool m_EraseMode = false;

        float m_PaletteScale = 1.0f;
        char m_NewLayerName[128] = "New Layer";

        sf::Vector2i m_LastPaintedCell{-1, -1};
        bool m_LastLeftDown = false;
        bool m_LastRightDown = false;
    };
}
