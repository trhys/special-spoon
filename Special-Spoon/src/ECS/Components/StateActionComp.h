#pragma once

#include "Component.h"
#include "Core/Registers/StateRegistry.h"

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
            const char* editWindow = "Edit Mapping";
            const char* newWindow = "New Mapping";
            static std::string editingAction = "";
            static char actionStateBuf[64];
            static char stateStringBuf[64];
            actionRegistry = ActionRegistry::Get();
            stateRegistry = StateRegistry::Get();

            for(auto& [action, state] : m_Actions)
            {
                ImGui::Text("Action: %s", actionRegistry.GetName(action));
                ImGui::Text("State: %s", stateRegistry.GetName(state));
                if(ImGui::Button("Edit"))
                {
                    editingAction = action;
                    strncpy(actionStateBuf, action.c_str(), sizeof(actionStateBuf));
                    strncpy(stateStringBuf, state.c_str(), sizeof(stateStringBuf));
                    ImGui::OpenPopup(editWindow);
                }
            }
            if(ImGui::BeginPopup(editWindow))
            {
              // edit to select action and state from listbox - see inputcomp for reference
                
                ImGui::InputText("New Action: ", actionStateBuf, IM_ARRAYSIZE(actionStateBuf));
                ImGui::InputText("New State: ", stateStringBuf, IM_ARRAYSIZE(stateStringBuf));
                if(ImGui::Button("Submit"))
                {
                    m_Actions.erase(editingAction);
                    m_Actions[actionStateBuf] = stateStringBuf;
                    actionStateBuf[0] = '\0';
                    stateStringBuf[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::Separator();
            if(ImGui::Button("Add New"))
            {
                ImGui::OpenPopup(newWindow);
                if(ImGui::BeginPopup(newWindow))
                {
                    static char actionStateBuf[64];
                    static char stateStringBuf[64];
                    ImGui::InputText("New Action String: ", actionStateBuf, IM_ARRAYSIZE(actionStateBuf));
                    ImGui::InputText("New State String: ", stateStringBuf, IM_ARRAYSIZE(stateStringBuf));
                    if(ImGui::Button("Submit"))
                    {
                        m_Actions[actionStateBuf] = stateStringBuf;
                        actionStateBuf[0] = '\0';
                        stateStringBuf[0] = '\0';
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StateActionComp, m_Actions)
}
