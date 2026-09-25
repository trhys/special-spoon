#pragma once

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"

#include <functional>

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

        // ray picked entity overlay
        bool selectedEntityOverlay = false;
        static OverlayCmd selectedEntityOverlayCmd;

        // displays an outline on all colliders in the scene
        bool colliderOverlay = false;
        static OverlayCmd colliderOverlayCmd;
    };
}
