#pragma once

#include "ECS/Components/Component.h"

#include "SFML/Window/Keyboard.hpp"

#include <unordered_map>

namespace Spoon
{
    struct InputComp : public Component
    {
        InputComp(const std::unordered_map<std::string, std::string>& keyBindings)
            : m_KeyBindings(keyBindings) {}

        std::unordered_map<std::string, std::string> m_KeyBindings;
        sf::Keyboard::Key pressedKey;
        bool keyIsPressed = false;
    };
}