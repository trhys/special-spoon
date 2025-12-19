#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Color.hpp"

namespace Spoon
{
    struct ColorComp : public ComponentBase<ColorComp>
    {
        ColorComp(sf::Color color = sf::Color::White) : ComponentBase::ComponentBase("ColorComp"), m_Color(color) {}

        sf::Color m_Color;

        void SetColor(sf::Color color) { m_Color = color; }

        void SetAlpha(std::uint8_t alpha) { m_Color.a = alpha; }

        void OnReflect() override
        {
            float color[4] = {
                m_Color.r / 255.0f,
                m_Color.g / 255.0f,
                m_Color.b / 255.0f,
                m_Color.a / 255.0f
            };

            if (ImGui::ColorEdit4("Color", color))
            {
                m_Color.r = static_cast<std::uint8_t>(color[0] * 255.0f);
                m_Color.g = static_cast<std::uint8_t>(color[1] * 255.0f);
                m_Color.b = static_cast<std::uint8_t>(color[2] * 255.0f);
                m_Color.a = static_cast<std::uint8_t>(color[3] * 255.0f);
            }
        }
    };
}