#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct StatusComp : public ComponentBase<StatusComp>
    {
        StatusComp(bool isActive = true, std::string currentState = "idle")
            : ComponentBase::ComponentBase(Name), is_Active(isActive), m_CurrentState(currentState) {}

        static constexpr const char* Name = "Status";
        bool is_Active;
        std::string m_CurrentState; // Idle, Moving, Attacking, etc. This maps to AnimationMap
        
        void OnReflect() override
        {
            ImGui::Text("Current state: %s", m_CurrentState.c_str());
            ImGui::Checkbox("Is Active", &is_Active);
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StatusComp, is_Active, m_CurrentState)
}