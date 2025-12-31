#pragma once

#include "Core/Core.h"
#include "ECS/Components/Animation/AnimationComp.h"
#include "ECS/Components/Animation/AnimationData.h"
#include "ECS/Components/Render/SpriteComp.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    struct AnimationData;
    struct AnimationComp;
    struct SpriteComp;

    class AnimationTool
    {
    public:
        void Update(sf::Time tick);
        void Open(AnimationData* data);
        void Animate(sf::Time tick);
        bool IsOpen() { return m_isOpen; }

        void Shutdown();

        void CreateNew();
        void EditTool();

    private:
        AnimationData* currentData = nullptr;
        bool m_isOpen = false;

        // Animation/Editor controls
        SpriteComp previewSprite;
        SpriteComp editorSprite;
        bool m_Looping = true;
        bool m_Playback = false;
        bool autoPlayEnabled = true;
        int frameIndex = 0;

        // Animation Component pieces
        int currentFrame = 0;
        float elapsedTime = 0.0f;
        bool isFinished = false;

        // Menu flags
        bool createModal = false;
        bool editTool = false;

        sf::RenderTexture m_Viewport;
        sf::View m_Camera;

        sf::RenderTexture m_EditorTool;
        sf::View m_EditorCamera;
    };
}