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
            
        }
    };
}