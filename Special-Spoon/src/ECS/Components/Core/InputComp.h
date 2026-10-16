#pragma once

#include "ECS/Components/Component.h"
#include "Core/Registers/ActionRegistry.h"
#include "Utils/Macros.h"

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <variant>

namespace Spoon
{
    enum class InputTrigger
    {
        Pressed,  // Once when the input transitions from up to down.
        Released, // Once when the input transitions from down to up.
        Held,     // On each input-system update while the input is down.
        Repeat    // Immediately on press, then at RepeatIntervalSeconds while down.
    };

    struct InputBinding
    {
        std::variant<sf::Keyboard::Key, sf::Mouse::Button> source = sf::Keyboard::Key::A;
        ActionType action;
        InputTrigger trigger = InputTrigger::Pressed;
        float repeatIntervalSeconds = 0.2f;

        // Runtime state for handling input events.
        bool isPressed = false;
        float timer = 0.0f;

        bool HasValidSource() const
        {
            if (const auto* key = std::get_if<sf::Keyboard::Key>(&source))
            {
                const int value = static_cast<int>(*key);
                return value >= 0 && value < static_cast<int>(sf::Keyboard::KeyCount);
            }

            const auto* button = std::get_if<sf::Mouse::Button>(&source);
            if (!button)
                return false;
            const int value = static_cast<int>(*button);
            return value >= 0 && value < static_cast<int>(sf::Mouse::ButtonCount);
        }

        bool IsValid() const
        {
            const int triggerValue = static_cast<int>(trigger);
            return HasValidSource()
                && triggerValue >= static_cast<int>(InputTrigger::Pressed)
                && triggerValue <= static_cast<int>(InputTrigger::Repeat)
                && std::isfinite(repeatIntervalSeconds)
                && repeatIntervalSeconds > 0.0f;
        }

        void ResetRuntimeState()
        {
            isPressed = false;
            timer = 0.0f;
        }
    };

    struct InputComp : public ComponentBase<InputComp>
    {
        InputComp() : ComponentBase::ComponentBase(Name) {}

        static constexpr const char* Name = "Input";

        std::vector<InputBinding> m_Bindings;
        void OnReflect() override;
    };

    inline void to_json(json& j, const InputBinding& binding)
    {
        j = json{
            {"Key", std::holds_alternative<sf::Keyboard::Key>(binding.source)
                ? static_cast<int>(std::get<sf::Keyboard::Key>(binding.source)) : -1},
            {"Button", std::holds_alternative<sf::Mouse::Button>(binding.source)
                ? static_cast<int>(std::get<sf::Mouse::Button>(binding.source)) : -1},
            {"Action", binding.action},
            {"Trigger", static_cast<int>(binding.trigger)},
            {"RepeatIntervalSeconds", binding.repeatIntervalSeconds}
        };
    }

    inline void from_json(const json& j, InputBinding& binding)
    {
        if (j.contains("Key"))
        {
            int key = j.at("Key").get<int>();
            if (key >= 0 && key < static_cast<int>(sf::Keyboard::KeyCount))
                binding.source = static_cast<sf::Keyboard::Key>(key);
        }
        else {
			binding.source = sf::Keyboard::Key::Unknown;
        }
        if (j.contains("Button"))
        {
            int button = j.at("Button").get<int>();
            if (button >= 0 && button < static_cast<int>(sf::Mouse::ButtonCount))
                binding.source = static_cast<sf::Mouse::Button>(button);
        }
        if (j.contains("Action"))
            binding.action = j.at("Action").get<ActionType>();
        if (j.contains("Trigger"))
            binding.trigger = static_cast<InputTrigger>(j.at("Trigger").get<int>());
        if (j.contains("RepeatIntervalSeconds"))
            binding.repeatIntervalSeconds = j.at("RepeatIntervalSeconds").get<float>();
    }

    inline void to_json(json& j, const InputComp& comp)
    {
        j["m_Bindings"] = json::array();
        for (const auto& binding : comp.m_Bindings)
        {
            if (!binding.IsValid())
            {
                SS_DEBUG_LOG("[InputComp] Skipping invalid input binding during serialization.");
                continue;
            }

            j["m_Bindings"].push_back(
            {
                {"Key", std::holds_alternative<sf::Keyboard::Key>(binding.source)
                    ? static_cast<int>(std::get<sf::Keyboard::Key>(binding.source)) : -1},
                {"Button", std::holds_alternative<sf::Mouse::Button>(binding.source)
                    ? static_cast<int>(std::get<sf::Mouse::Button>(binding.source)) : -1},
                {"Action", binding.action},
                {"Trigger", static_cast<int>(binding.trigger)},
                {"RepeatIntervalSeconds", binding.repeatIntervalSeconds}
            });
        }
    }

    inline void from_json(const json& j, InputComp& comp)
    {
        comp.m_Bindings.clear();
        if (!j.is_object())
        {
            SS_DEBUG_LOG("[InputComp] Ignoring malformed input component JSON.");
            return;
        }

        if (j.contains("m_Bindings") && j.at("m_Bindings").is_array())
        {
            std::vector<InputBinding> bindings;
            for (const auto& binding : j.at("m_Bindings"))
            {
                InputBinding newBinding = binding.get<InputBinding>();
                if (newBinding.IsValid())
                    bindings.push_back(newBinding);
            }
            comp.m_Bindings = std::move(bindings);
        }
    }
}   