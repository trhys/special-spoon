#pragma once

#include "Component.h"
#include "Core/Registers/StateRegistry.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
    struct StateActionComp : public ComponentBase<StateActionComp>
    {
        StateActionComp() : ComponentBase::ComponentBase(Name) {}
        StateActionComp(std::unordered_map<ActionType, StateType> stateActions) : ComponentBase::ComponentBase(Name), m_Actions(stateActions) {}

        static constexpr const char* Name = "StateAction";
        std::unordered_map<ActionType, StateType> m_Actions;
        
        void OnReflect() override
        {
            static ActionType currentAction = 0;  // Currently selected action
            static ActionType editAction = 0;     // Action to replace
            static StateType editState = 0;       // State to replace

            ImGui::SeparatorText("Current Action-State Mappings");
            if(ImGui::BeginListBox("##ActionMappings"))
            {
                for(auto& [action, state] : m_Actions)
                {
                    std::string actionName = ActionRegistry::Get().GetName(action);
                    ImGui::PushID(actionName.c_str());
                    const bool is_selected = (currentAction == action);
                    if(ImGui::Selectable(actionName.c_str(), is_selected))
                    {
                        currentAction = action;
                    }
                    if(is_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }
            ImGui::SameLine(); HelpMarker("Select an action to edit its state mapping.");

            ImGui::SeparatorText("Action State Mapping");
            if(currentAction != 0 && m_Actions.count(currentAction))
            {
                const char* editWindow = "Edit Mapping";

                std::string actionName = ActionRegistry::Get().GetName(currentAction);
                std::string stateName = StateRegistry::Get().GetName(m_Actions[currentAction]);
                ImGui::Text("Action: %s", actionName.c_str());
                ImGui::Text("State: %s", stateName.c_str());
                ImGui::SameLine(); if(ImGui::Button("Edit")) ImGui::OpenPopup(editWindow);

                if(ImGui::BeginPopup(editWindow))
                {
                    ImGui::BeginChild("Available Actions", ImVec2(200, 300), true);
                    if(ImGui::BeginListBox("##actionlist", ImVec2(-FLT_MIN, -FLT_MIN)))
                    {
                        for(const auto& [name, id] : ActionRegistry::Get().m_NameToID)
                        {
                            const char* actionName = name.c_str();
                            ImGui::PushID(actionName);
                            const bool is_selected = (editAction == id);
                            if(ImGui::Selectable(actionName, is_selected))
                            {
                                editAction = id;
                            }
                            if(is_selected)
                                ImGui::SetItemDefaultFocus();
                            ImGui::PopID();
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("Available States", ImVec2(200, 300), true);
                    if(ImGui::BeginListBox("##statelist", ImVec2(-FLT_MIN, -FLT_MIN)))
                    {
                        for(const auto& [name, id] : StateRegistry::Get().m_NameToID)
                        {
                            const char* stateName = name.c_str();
                            ImGui::PushID(stateName);
                            const bool is_selected = (editState == id);
                            if(ImGui::Selectable(stateName, is_selected))
                            {
                                editState = id;
                            }
                            if(is_selected)
                                ImGui::SetItemDefaultFocus();
                            ImGui::PopID();
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::EndChild();

                    if(ImGui::Button("Submit"))
                    {
                        m_Actions.erase(currentAction);
                        m_Actions[editAction] = editState;
                        currentAction = 0;
                        editAction = 0;
                        editState = 0;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if(ImGui::Button("Cancel"))
                    {
                        editAction = 0;
                        editState = 0;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            else
            {
                ImGui::TextDisabled("No mapping available");
            }

            const char* newMapWindow = "New Mapping";
            if(ImGui::Button("Add New"))
                ImGui::OpenPopup(newMapWindow);

            if(ImGui::BeginPopup(newMapWindow))
            {
                ImGui::BeginChild("Available Actions", ImVec2(200, 300), true);
                if(ImGui::BeginListBox("##actionlist", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for(const auto& [name, id] : ActionRegistry::Get().m_NameToID)
                    {
                        const char* actionName = name.c_str();
                        ImGui::PushID(actionName);
                        const bool is_selected = (editAction == id);
                        if(ImGui::Selectable(actionName, is_selected))
                        {
                            editAction = id;
                        }
                        if(is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::BeginChild("Available States", ImVec2(200, 300), true);
                if(ImGui::BeginListBox("##statelist", ImVec2(-FLT_MIN, -FLT_MIN)))
                {
                    for(const auto& [name, id] : StateRegistry::Get().m_NameToID)
                    {
                        const char* stateName = name.c_str();
                        ImGui::PushID(stateName);
                        const bool is_selected = (editState == id);
                        if(ImGui::Selectable(stateName, is_selected))
                        {
                            editState = id;
                        }
                        if(is_selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();

                if(ImGui::Button("Submit"))
                {
                    m_Actions[editAction] = editState;
                    editAction = 0;
                    editState = 0;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StateActionComp, m_Actions)
}
