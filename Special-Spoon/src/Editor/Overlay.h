#pragma once

#include <functional>
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"

namespace Spoon
{
    struct OverlayCmd
    {
        std::function<void(sf::RenderTarget&, sf::RenderStates)> draw;
    };

    class Overlay
    {
    public:
        Overlay() {}
        ~Overlay() {}

        void PushOverlay();

        // displays an outline on all colliders in the scene
        bool colliderOverlay = false;
        static OverlayCmd colliderOverlayCmd;
    };
}