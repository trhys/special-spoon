#pragma once

#include "ECS/UUID.h"
#include <optional>

namespace Spoon
{
    // tracks cursor navigation mode
    enum class CursorMode {
		Pointer, // mouse or touch
		Keyboard // keyboard or gamepad
    };

    struct UICursor {
        std::optional<UUID> hoveredComp;
        std::optional<UUID> focusedComp;
		CursorMode mode = CursorMode::Pointer;

        void Reset()
        {
            hoveredComp.reset();
            focusedComp.reset();
			mode = CursorMode::Pointer;
        }
    };
}