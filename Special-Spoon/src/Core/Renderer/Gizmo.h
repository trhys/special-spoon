#pragma once

#include <functional>
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"

namespace Spoon
{
    struct GizmoCommand
    {
        std::function<void(
            sf::RenderTarget&,
            sf::RenderStates
            )> draw;
    };
}