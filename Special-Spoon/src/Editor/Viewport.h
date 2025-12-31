#pragma once

#include "SFML/Graphics.hpp"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    struct ViewportState
    {
        sf::Vector2f mouseDrag;
        bool dragging = false;
    };

    struct Viewport
    {
        sf::RenderTexture target;
        sf::View camera;
        ViewportState state;
    };

    void RenderViewport(Viewport& viewport);
}