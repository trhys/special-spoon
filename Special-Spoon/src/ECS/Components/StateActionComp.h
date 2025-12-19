#pragma once

#include "Component.h"

namespace Spoon
{
    struct StateActionComp : public ComponentBase<StateActionComp>
    {
        StateActionComp(std::unordered_map<std::string, std::string> stateActions) : ComponentBase::ComponentBase("StateActionComp"), m_Actions(stateActions) {}

        std::unordered_map<std::string, std::string> m_Actions;

        void OnReflect() override
        {
            const char* editWindow = "Edit Mapping";
            const char* newWindow = "New Mapping";
            for(auto& [action, state] : m_Actions)
            {
                ImGui::Text("Action String: %s", action.c_str());
                ImGui::Text("State String: %s", state.c_str());
                if(ImGui::Button("Edit"))
                {
                    ImGui::OpenPopup(editWindow);
                    if(ImGui::BeginPopup(editWindow))
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
}