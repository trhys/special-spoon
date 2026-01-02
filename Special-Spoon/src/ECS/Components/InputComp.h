#pragma once

#include "ECS/Components/Component.h"
#include "Editor/Editor.h"

#include "SFML/Window/Keyboard.hpp"

#include <unordered_map>

namespace Spoon
{
    struct InputComp : public ComponentBase<InputComp>
    {
        InputComp() : ComponentBase::ComponentBase("Input") {}
        InputComp(const std::unordered_map<std::string, std::string>& keyBindings)
            : ComponentBase::ComponentBase("Input"), m_KeyBindings(keyBindings) 
            {
                for (const auto& pair : keyBindings)
                {
                    m_KeyStates[pair.first] = false;
                }
            }

        std::unordered_map<std::string, std::string> m_KeyBindings;
        std::unordered_map<std::string, bool> m_KeyStates;

        void OnReflect() override
        {
            static std::string currentKey = "";
            if(ImGui::BeginListBox("Key Bindings"))
            {
                for(auto& [key, action] : m_KeyBindings)
                {   
                    ImGui::PushID(key.c_str());
                    const bool is_selected = (currentKey == key);
                    if(ImGui::Selectable(key.c_str(), is_selected))
                    {
                        currentKey = key;
                    }
                    if(is_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::SameLine(); HelpMarker("Key bindings maps a keypress to an action string\nthat gets sent to the action buffer");
            
            ImGui::SeparatorText("Action String Mapping");
            if(!currentKey.empty() && m_KeyBindings.count(currentKey))
            {
                const char* editWindow = "Edit Binding";

                ImGui::Text("Key: %s", currentKey.c_str());
                ImGui::Text("Action: %s", m_KeyBindings[currentKey].c_str());
                ImGui::SameLine(); if(ImGui::Button("Edit")) ImGui::OpenPopup(editWindow);

                if(ImGui::BeginPopup(editWindow))
                {   
                    static char actionStringbuf[64];
                    static char keyStringbuf[64];
                    ImGui::InputText("New Key", keyStringbuf, IM_ARRAYSIZE(keyStringbuf));
                    ImGui::InputText("New Action String", actionStringbuf, IM_ARRAYSIZE(actionStringbuf));
                    if(ImGui::Button("Submit"))
                    {
                        if(keyStringbuf[0] != '\0' && actionStringbuf[0] != '\0')
                        {
                            std::string oldKey = currentKey;
                            currentKey = keyStringbuf;
                            m_KeyBindings[keyStringbuf] = actionStringbuf;
                            m_KeyBindings.erase(oldKey);
                            keyStringbuf[0] = '\0';
                            actionStringbuf[0] = '\0';
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::SameLine();
                    if(ImGui::Button("Delete"))
                    {
                        m_KeyBindings.erase(currentKey);
                        currentKey = "";
                        keyStringbuf[0] = '\0';
                        actionStringbuf[0] = '\0';
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
            if(ImGui::Button("New"))
            {
                ImGui::OpenPopup(newBindWindow);
            }
            if(ImGui::BeginPopup(newBindWindow))
            {
                static char newKeyBuf[64];
                static char newActionBuf[64];
                ImGui::InputText("New Key: ", newKeyBuf, IM_ARRAYSIZE(newKeyBuf));
                ImGui::InputText("New Action String: ", newActionBuf, IM_ARRAYSIZE(newActionBuf));
                if(ImGui::Button("Submit"))
                {
                    m_KeyBindings[newKeyBuf] = newActionBuf;
                    newKeyBuf[0] = '\0';
                    newActionBuf[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InputComp, m_KeyBindings)
}