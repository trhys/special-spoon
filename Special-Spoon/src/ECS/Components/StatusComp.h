#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct StatusComp : public ComponentBase<StatusComp>
    {
        StatusComp(bool isActive, std::string currentState)
            : ComponentBase::ComponentBase("StatusComp"), is_Active(isActive), m_CurrentState(currentState) 
        {
        }

        bool is_Active;
        std::string m_CurrentState; // Idle, Moving, Attacking, etc. This maps to AnimationMap

        void OnReflect() override
        {
            std::string active;
            if(is_Active)
                active = "Yes";
            else active = "No";

            ImGui::Text("Current state: %s", m_CurrentState.c_str());
            ImGui::Text("Active: %s", active.c_str());
            ImGui::SameLine(); ImGui::Checkbox("##Active", &is_Active);
        }
    };
}