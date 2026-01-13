#pragma once

#include "ECS/Components/Component.h"
#include "Editor/Editor.h"
#include "Core/Registers/ActionRegistry.h"

#include "SFML/Window/Keyboard.hpp"

namespace Spoon
{
    struct InputComp : public ComponentBase<InputComp>
    {
        InputComp() : ComponentBase::ComponentBase(Name) {}
        InputComp(const std::unordered_map<sf::Keyboard::Key, ActionType>& keyBindings)
            : ComponentBase::ComponentBase(Name), m_KeyBindings(keyBindings) 
            {
                for (const auto& pair : keyBindings)
                {
                    m_KeyStates[pair.first] = false;
                }
            }

        static constexpr const char* Name = "Input";

        // Key: Action string mapping
        std::unordered_map<sf::Keyboard::Key, ActionType> m_KeyBindings;

        // Key: Pressed state mapping
        std::unordered_map<sf::Keyboard::Key, bool> m_KeyStates;

        void OnReflect() override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputComp, m_KeyBindings)
}