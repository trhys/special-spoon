#pragma once

#include "ECS/Components/Component.h"

#include "SFML/Window/Keyboard.hpp"

#include <unordered_map>

namespace Spoon
{
    struct InputComp : public Component
    {
        InputComp(const std::unordered_map<std::string, std::string>& keyBindings)
            : m_KeyBindings(keyBindings) 
            {
                for (const auto& pair : keyBindings)
                {
                    m_KeyStates[pair.first] = false;
                }
            }

        std::unordered_map<std::string, std::string> m_KeyBindings;
        std::unordered_map<std::string, bool> m_KeyStates;
    };
}