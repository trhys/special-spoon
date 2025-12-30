#pragma once

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace sf
{
    class View;
}

namespace Spoon
{
    void RenderViewport(sf::RenderTexture& target, sf::View& camera);
}