#include "InputComp.h"

#include "Imgui/imgui.h"

#include <cmath>
#include <cstddef>
#include <string>

namespace Spoon
{
    namespace
    {
        const char* MouseButtonToString(sf::Mouse::Button button)
        {
            switch (button)
            {
            case sf::Mouse::Button::Left: return "Left";
            case sf::Mouse::Button::Right: return "Right";
            case sf::Mouse::Button::Middle: return "Middle";
            case sf::Mouse::Button::Extra1: return "X1";
            case sf::Mouse::Button::Extra2: return "X2";
            }
            return "Unknown";
        }
    }

    void InputComp::OnReflect()
    {
        static constexpr const char* triggerNames[] = {
            "Pressed", "Released", "Held", "Repeat"
        };
        static constexpr const char* triggerDescriptions[] = {
            "Emits once when the input is pressed.",
            "Emits once when the input is released.",
            "Emits on each input-system update while held.",
            "Emits on press, then at the repeat interval while held."
        };
        constexpr int triggerCount = static_cast<int>(IM_ARRAYSIZE(triggerNames));

        ImGui::SeparatorText("Input Bindings");
        int newBindingIndex = -1;
        if (ImGui::Button("Add Binding"))
        {
            m_Bindings.emplace_back();
            newBindingIndex = static_cast<int>(m_Bindings.size()) - 1;
        }

        for (std::size_t i = 0; i < m_Bindings.size(); ++i)
        {
            InputBinding& binding = m_Bindings[i];
            bool bindingChanged = false;
            if (!binding.HasValidSource())
            {
                binding.source = sf::Keyboard::Key::A;
                bindingChanged = true;
            }
            const int triggerValue = static_cast<int>(binding.trigger);
            if (triggerValue < static_cast<int>(InputTrigger::Pressed)
                || triggerValue > static_cast<int>(InputTrigger::Repeat))
            {
                binding.trigger = InputTrigger::Pressed;
                bindingChanged = true;
            }
            if (!std::isfinite(binding.repeatIntervalSeconds) || binding.repeatIntervalSeconds <= 0.0f)
            {
                binding.repeatIntervalSeconds = 0.2f;
                bindingChanged = true;
            }

            ImGui::PushID(static_cast<int>(i));
            if (static_cast<int>(i) == newBindingIndex)
                ImGui::SetNextItemOpen(true, ImGuiCond_Always);

            const std::string label = "Binding " + std::to_string(i + 1);
            if (ImGui::CollapsingHeader(label.c_str()))
            {
                bool isMouseBinding = std::holds_alternative<sf::Mouse::Button>(binding.source);
                if (ImGui::BeginCombo("Input Type", isMouseBinding ? "Mouse Button" : "Keyboard Key"))
                {
                    if (ImGui::Selectable("Keyboard Key", !isMouseBinding) && isMouseBinding)
                    {
                        isMouseBinding = false;
                        binding.source = sf::Keyboard::Key::A;
                        bindingChanged = true;
                    }
                    if (ImGui::Selectable("Mouse Button", isMouseBinding) && !isMouseBinding)
                    {
                        isMouseBinding = true;
                        binding.source = sf::Mouse::Button::Left;
                        bindingChanged = true;
                    }
                    ImGui::EndCombo();
                }

                if (isMouseBinding)
                {
                    const sf::Mouse::Button currentButton = std::get<sf::Mouse::Button>(binding.source);
                    const char* buttonName = MouseButtonToString(currentButton);
                    if (ImGui::BeginCombo("Mouse Button", buttonName))
                    {
                        for (int button = static_cast<int>(sf::Mouse::Button::Left);
                            button <= static_cast<int>(sf::Mouse::Button::Extra2); ++button)
                        {
                            const auto mouseButton = static_cast<sf::Mouse::Button>(button);
                            if (ImGui::Selectable(MouseButtonToString(mouseButton),
                                currentButton == mouseButton) && currentButton != mouseButton)
                            {
                                binding.source = mouseButton;
                                bindingChanged = true;
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
                else
                {
                    const sf::Keyboard::Key currentKey = std::get<sf::Keyboard::Key>(binding.source);
                    const std::string keyName = sf::Keyboard::getDescription(
                        sf::Keyboard::delocalize(currentKey)).toAnsiString();
                    if (ImGui::BeginCombo("Keyboard Key", keyName.c_str()))
                    {
                        for (int key = static_cast<int>(sf::Keyboard::Key::A);
                            key < static_cast<int>(sf::Keyboard::KeyCount); ++key)
                        {
                            const auto keyboardKey = static_cast<sf::Keyboard::Key>(key);
                            const std::string name = sf::Keyboard::getDescription(
                                sf::Keyboard::delocalize(keyboardKey)).toAnsiString();
                            if (ImGui::Selectable(name.c_str(), currentKey == keyboardKey)
                                && currentKey != keyboardKey)
                            {
                                binding.source = keyboardKey;
                                bindingChanged = true;
                            }
                        }
                        ImGui::EndCombo();
                    }
                }

                const auto& actionNames = ActionRegistry::Get().m_IDToName;
                const auto currentAction = actionNames.find(binding.action.m_ID);
                const char* actionName = currentAction == actionNames.end()
                    ? "Select action"
                    : currentAction->second.c_str();
                if (ImGui::BeginCombo("Action", actionName))
                {
                    for (const auto& [id, name] : actionNames)
                    {
                        if (ImGui::Selectable(name.c_str(), binding.action.m_ID == id)
                            && binding.action.m_ID != id)
                        {
                            binding.action = ActionType{ id };
                            bindingChanged = true;
                        }
                    }
                    ImGui::EndCombo();
                }

                const int trigger = static_cast<int>(binding.trigger);
                const char* triggerName = trigger >= 0 && trigger < triggerCount
                    ? triggerNames[trigger]
                    : "Unknown";
                if (ImGui::BeginCombo("Trigger", triggerName))
                {
                    for (int option = 0; option < triggerCount; ++option)
                    {
                        if (ImGui::Selectable(triggerNames[option], trigger == option) && trigger != option)
                        {
                            binding.trigger = static_cast<InputTrigger>(option);
                            bindingChanged = true;
                        }
                    }
                    ImGui::EndCombo();
                }
                if (trigger >= 0 && trigger < triggerCount)
                    ImGui::TextDisabled("%s", triggerDescriptions[trigger]);

                if (binding.trigger == InputTrigger::Repeat)
                {
                    float repeatInterval = binding.repeatIntervalSeconds;
                    if (ImGui::InputFloat("Repeat Interval (seconds)", &repeatInterval, 0.01f, 0.1f)
                        && std::isfinite(repeatInterval) && repeatInterval > 0.0f)
                    {
                        binding.repeatIntervalSeconds = repeatInterval;
                        bindingChanged = true;
                    }
                    else if (!std::isfinite(repeatInterval) || repeatInterval <= 0.0f)
                    {
                        ImGui::TextDisabled("Repeat interval must be greater than zero.");
                    }
                }
                if (ImGui::Button("Remove Binding"))
                {
                    m_Bindings.erase(m_Bindings.begin() + static_cast<std::ptrdiff_t>(i));
                    ImGui::PopID();
                    break;
                }
            }
            if (bindingChanged)
                binding.ResetRuntimeState();
            ImGui::PopID();
        }
    }
}
