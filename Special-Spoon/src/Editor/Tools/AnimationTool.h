#pragma once

#include "Core/Core.h"
#include "ECS/Components/Animation/AnimationComp.h"
#include "ECS/Components/Render/SpriteComp.h"
#include "Editor/Utils/Viewport.h"
#include "Utils/Graphics/GridSlicer.h"

namespace Spoon
{
    struct AnimationData;
    struct AnimationComp;

    class AnimationTool
    {
    public:
        void Update(sf::Time tick);
        void Open(AnimationData* data);
        void Animate(sf::Time tick);
        bool IsOpen() { return m_isOpen; }

        void Shutdown();

        void CreateNew();

    private:
        AnimationData* currentData = nullptr;
        bool m_isOpen = false;

        enum class ToolMode
        {
            Manual = 0, // use manual rect drawing
            Grid = 1    // slice the atlas into grid palette
        };

        // Animation/Editor controls
        SpriteComp previewSprite;
        SpriteComp editorSprite;
        sf::IntRect rect;
        sf::Vector2i m_DragStart;
        bool m_Dragging = false;
        bool m_Looping = true;
        bool m_Playback = false;
        int frameIndex = 0;
        AtlasGrid m_Grid;

        // Animation Component pieces
        int currentFrame = 0;
        float elapsedTime = 0.0f;
        bool isFinished = false;

        // Menu flags
        bool createModal = false;
        bool editTool = false;

        // Settings
        bool autoPlayEnabled = true;
        bool showToolTips = true;
        ToolMode toolMode = ToolMode::Manual;
        bool m_ShowGridOverlay = true;
        bool m_GridClampToTexture = true;
        bool m_GridAppendRowMajor = true;
        int m_SelectedGridIndex = -1;
        int m_GridRangeStart = 0;
        int m_GridRangeCount = 1;

        Viewport m_MainVP;
        Viewport m_EditorVP;

    private:
        void DrawGridSlicerTools();
        void DrawGridOverlay(const ImVec2& viewportPos);
        void HandleViewport(const ImVec2& viewportPos);
        bool CanUseGrid() const;
        void ApplyGridSelectionToRect();
        void AddGridRangeFrames();
    };
}