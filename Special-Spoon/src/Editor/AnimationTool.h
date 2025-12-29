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

    private:
        AnimationData* currentData = nullptr;
        bool m_isOpen = false;

        // Animation controls
        SpriteComp previewSprite;
        bool m_Looping = true;
        bool m_Playback = false;

        // Animation Component pieces
        int currentFrame = 0;
        float elapsedTime = 0.0f;
        bool isFinished = false;

        bool createModal = false;

        sf::RenderTexture m_Viewport;
        sf::View m_Camera;
    };
}