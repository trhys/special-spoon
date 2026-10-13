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
                for (const auto& pair : m_MouseButtonBindings)
                {
                    m_MouseButtonStates[pair.first] = false;
                }
            }

        static constexpr const char* Name = "Input";

        // Key: Action string mapping
        std::unordered_map<sf::Keyboard::Key, ActionType> m_KeyBindings;
        std::unordered_map<sf::Keyboard::Key, bool> m_KeyStates;

        // mouse input
        std::unordered_map<sf::Mouse::Button, ActionType> m_MouseButtonBindings;
        std::unordered_map<sf::Mouse::Button, bool> m_MouseButtonStates;

        void OnReflect() override;
    };

    inline void to_json(json& j, const InputComp& comp)
    {
        j["m_KeyBindings"] = json::array();
        for (const auto& [key, action] : comp.m_KeyBindings)
        {
            j["m_KeyBindings"].push_back(
            {
                {"Key", static_cast<int>(key)},
                {"Action", action}
            });
        }
        j["m_MouseButtonBindings"] = json::array();
        for (const auto& [button, action] : comp.m_MouseButtonBindings)
        {
            j["m_MouseButtonBindings"].push_back(
            {
                {"Button", static_cast<int>(button)},
                {"Action", action}
            });
        }
    }

    inline void from_json(const json& j, InputComp& comp)
    {
        comp.m_KeyBindings.clear();
        if (!j.contains("m_KeyBindings") || !j["m_KeyBindings"].is_array())
            return;

        for (const auto& binding : j["m_KeyBindings"])
        {
            sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(binding.at("Key").get<int>());
            ActionType action = binding.at("Action").get<ActionType>();
            comp.m_KeyBindings[key] = action;
        }
        comp.m_MouseButtonBindings.clear();
        if (!j.contains("m_MouseButtonBindings") || !j["m_MouseButtonBindings"].is_array())
            return;

        for (const auto& binding : j["m_MouseButtonBindings"])
        {
            sf::Mouse::Button button = static_cast<sf::Mouse::Button>(binding.at("Button").get<int>());
            ActionType action = binding.at("Action").get<ActionType>();
            comp.m_MouseButtonBindings[button] = action;
        }
    }
}