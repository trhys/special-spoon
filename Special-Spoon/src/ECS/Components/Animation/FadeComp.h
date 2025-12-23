#pragma once

#include "ECS/Components/Component.h"
#include "SFML/System/Time.hpp"

namespace Spoon
{
    struct FadeComp : public ComponentBase<FadeComp>
    {
        FadeComp(float rate = 2.0f) : ComponentBase::ComponentBase("Fade"), m_FadeRate(rate) {}

        sf::Time m_FadeTimer;
        float m_Alpha;
        float m_FadeRate;
        bool isFadingIn;
        bool isFadingOut;
        bool isActive = true;

        void FadeIn() 
        { 
            isFadingIn = true;
            isFadingOut = false;
            m_Alpha = 0.0f;
            isActive = true;
        }

        void FadeOut() 
        { 
            isFadingOut = true;
            isFadingIn = false;
            m_Alpha = 255.0f;
            isActive = true;
        }

        void ToggleActive() { isActive = !isActive; }

        void OnReflect() override
        {
            ImGui::InputFloat("Current Fade Rate", &m_FadeRate, 0.1f);
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FadeComp, m_FadeRate)
}