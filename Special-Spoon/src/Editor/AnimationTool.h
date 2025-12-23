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
        static AnimationTool& Get()
        {
            if (!s_Instance) s_Instance = new AnimationTool();
            return *s_Instance;
        }

        void Update(sf::Time tick);
        void Open(AnimationData* data);
        void Animate(sf::Time tick);
        bool IsOpen() { return m_isOpen; }

        void Shutdown();

    private:
        AnimationTool() = default;
        static inline AnimationTool* s_Instance;
        AnimationData* currentData = nullptr;
        SpriteComp previewSprite;  // Holds texture and texture rect

        bool m_isOpen = false;
        bool m_Looping = true;
        bool m_Playback = false;

        // Animation Component pieces
        int currentFrame = 0;
        float elapsedTime = 0.0f;
        bool isFinished = false;

        sf::RenderTexture m_Viewport;
    };
}