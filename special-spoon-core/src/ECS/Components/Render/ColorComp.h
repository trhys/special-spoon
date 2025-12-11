#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct ColorComp : public Component
    {
        sf::Color m_Color;

        void SetColor(sf::Color color) { m_Color = color; }
        void SetAlpha(std::uint8_t alpha) { m_Color.a = alpha; }
    };
}