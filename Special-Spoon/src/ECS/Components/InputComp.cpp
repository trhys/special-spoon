#include "InputComp.h"
#include "Core/Application.h"

namespace Spoon
{
    void InputComp::OnReflect()
    {
        static sf::Keyboard::Key currentKey = sf::Keyboard::Key::Unknown; // Currently selected key
        static sf::Keyboard::Key editKey = sf::Keyboard::Key::Unknown;    // Key to replace binding
        static uint32_t editAction = 0;                                   // Action ID to replace binding
        ImGui::SeparatorText("Current Key Bindings");
        if(ImGui::BeginListBox("##Key Bindings"))
        {
            for(auto& [key, action] : m_KeyBindings)
            {   
                std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(key)).toAnsiString();
                ImGui::PushID(keyName.c_str());
                const bool is_selected = (currentKey == key);
                if(ImGui::Selectable(keyName.c_str(), is_selected))
                {
                    currentKey = key;
                }
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
                ImGui::PopID();
            }
            ImGui::EndListBox();
        }
        ImGui::SameLine(); HelpMarker("refactoring");
        
        ImGui::SeparatorText("Action String Mapping");
        if(currentKey != sf::Keyboard::Key::Unknown && m_KeyBindings.count(currentKey))
        {
            const char* editWindow = "Edit Binding";

            std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(currentKey)).toAnsiString();
            ImGui::Text("Key: %s", keyName.c_str());
            ImGui::Text("Action: %s", ActionRegistry::Get().GetName(m_KeyBindings[currentKey]).c_str());
            ImGui::SameLine(); if(ImGui::Button("Edit")) ImGui::OpenPopup(editWindow);

            if(ImGui::BeginPopup(editWindow))
            {   
                if (ImGui::BeginChild("Available Keys"))
                {
                    if (ImGui::BeginListBox("##keylist"))
                    {
                        for (int k = static_cast<int>(sf::Keyboard::Key::A); k <= static_cast<int>(sf::Keyboard::KeyCount); k++)
                        {
                            sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(k);
                            if (m_KeyBindings.find(key) != m_KeyBindings.end())
                                continue;

                            std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(key)).toAnsiString();
                            ImGui::PushID(keyName.c_str());
                            const bool is_selected = (editKey == key);
                            if (ImGui::Selectable(keyName.c_str(), is_selected))
                            {
                                editKey = key;
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                            ImGui::PopID();
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::EndChild();
                }
                if (ImGui::BeginChild("Available Actions"))
                {
                    if (ImGui::BeginListBox("##actionlist"))
                    {
                        for (const auto& [name, id] : ActionRegistry::Get().m_NameToID)
                        {
                            const char* actionName = name.c_str();
                            ImGui::PushID(actionName);
                            const bool is_selected = (editAction == id);
                            if (ImGui::Selectable(actionName, is_selected))
                            {
                                editAction = id;
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                            ImGui::PopID();
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::EndChild();
                }
                if(ImGui::Button("Submit"))
                {
                    m_KeyBindings.erase(currentKey);
                    m_KeyBindings[editKey] = ActionType{ editAction };
                    currentKey = sf::Keyboard::Key::Unknown;
                    editAction = 0;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if(ImGui::Button("Cancel"))
                {
                    editAction = 0;
                    editKey = sf::Keyboard::Key::Unknown;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextDisabled("No mapping available");
        }

        const char* newBindWindow = "New Binding";
        if (ImGui::Button("Add New"))
            ImGui::OpenPopup(newBindWindow);
        if (ImGui::BeginPopup(newBindWindow))
        {
            if (ImGui::BeginChild("Available Keys"))
            {
                if (ImGui::BeginListBox("##keylist"))
                {
                    for (int k = static_cast<int>(sf::Keyboard::Key::A); k <= static_cast<int>(sf::Keyboard::KeyCount); k++)
                    {
                        sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(k);
                        if (m_KeyBindings.find(key) != m_KeyBindings.end())
                            continue;

                        std::string keyName = sf::Keyboard::getDescription(static_cast<sf::Keyboard::Scancode>(key)).toAnsiString();
                        ImGui::PushID(keyName.c_str());
                        const bool is_selected = (editKey == key);
                        if (ImGui::Selectable(keyName.c_str(), is_selected))
                        {
                            editKey = key;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();
            }
            if (ImGui::BeginChild("Available Actions"))
            {
                if (ImGui::BeginListBox("##actionlist"))
                {
                    for (const auto& [name, id] : ActionRegistry::Get().m_NameToID)
                    {
                        const char* actionName = name.c_str();
                        ImGui::PushID(actionName);
                        const bool is_selected = (editAction == id);
                        if (ImGui::Selectable(actionName, is_selected))
                        {
                            editAction = id;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();
            }
            if(ImGui::Button("Submit"))
            {
                m_KeyBindings[editKey] = ActionType{ editAction };
                editAction = 0;
                editKey = sf::Keyboard::Key::Unknown;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}