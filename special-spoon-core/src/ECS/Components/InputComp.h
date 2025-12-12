#pragma once

#include "ECS/Components/Component.h"

#include <unordered_map>

namespace Spoon
{
    struct InputComp : public Component
    {
        InputComp(const std::unordered_map<std::string, std::string>& keyBindings)
            : m_KeyBindings(keyBindings) {}

        std::unordered_map<std::string, std::string> m_KeyBindings;
    };
}