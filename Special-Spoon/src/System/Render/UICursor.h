#pragma once

#include "ECS/UUID.h"
#include <optional>

namespace Spoon
{
    struct UICursor {
        std::optional<UUID> hoveredComp;
        std::optional<UUID> focusedComp;

        void Reset()
        {
            hoveredComp.reset();
            focusedComp.reset();
        }
    };
}