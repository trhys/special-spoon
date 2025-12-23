#pragma once

#include "ECS/Components/Component.h"
#include "SFML/System/Time.hpp"

namespace Spoon
{
    struct BlinkComp : public ComponentBase<BlinkComp>
    {
        BlinkComp(float rate = 2.0f) : ComponentBase::ComponentBase("Blink"), m_Blinkrate(rate) {}

        sf::Time m_BlinkTimer;
        float m_Alpha = 255.0f;
        float m_Blinkrate;
        bool isGoingUp = false;
        bool isActive = true;

        void SetBlinkRate(float rate) { m_Blinkrate = rate; }
        void ToggleSwitch() { isGoingUp = !isGoingUp; }
        void ToggleActive() { isActive = !isActive; }

        void OnReflect() override
        {
            ImGui::InputFloat("Current Blink Rate", &m_Blinkrate, 0.1f);
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlinkComp, m_Blinkrate)
}