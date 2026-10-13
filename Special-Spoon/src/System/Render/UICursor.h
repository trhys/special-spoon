#pragma once

#include "ECS/UUID.h"
#include <optional>

namespace Spoon
{
    struct UIIntent {
        bool select = false;
        bool cancel = false;
        bool navigate = false;
    };

    struct UICursor {
        std::optional<UUID> hoveredComp;
        std::optional<UUID> focusedComp;
        std::optional<UUID> pressedComp;
        UIIntent intent;
        bool selectHeldLastFrame = false;

        void Reset()
        {
            hoveredComp.reset();
            focusedComp.reset();
            pressedComp.reset();
            intent = UIIntent{};
        }

    };
}