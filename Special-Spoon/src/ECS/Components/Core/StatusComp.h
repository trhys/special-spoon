#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct SPOON_API StatusComp : public ComponentBase<StatusComp>
    {
        StatusComp(bool isActive = true, StateType currentState = StateType{BuiltInStates::Idle})
            : ComponentBase::ComponentBase(Name), is_Active(isActive), m_CurrentState(currentState) {}

        static constexpr const char* Name = "Status";

        bool is_Active;
        bool resolvedThisFrame = false; // did an upstream system set this
        StateType m_CurrentState; // Idle, Moving, Attacking, etc.
        
        void OnReflect() override;
        void ApplyState(StateType newState);
    };

    inline void to_json(json& j, const StatusComp& comp)
    {
        j["is_Active"] = comp.is_Active;
        j["m_CurrentState"] = comp.m_CurrentState;
    }

    inline void from_json(const json& j, StatusComp& comp)
    {        

        // No actions to load anymore, only is_Active and m_CurrentState
        comp.is_Active = j.at("is_Active").get<bool>();
        if (j.contains("m_CurrentState"))
        {
            comp.m_CurrentState = j.at("m_CurrentState").get<StateType>();
        }
        else
        {
            comp.m_CurrentState = StateType{ BuiltInStates::Idle }; // Default to Idle on load
        }
    }
}