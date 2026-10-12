#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct StatusComp : public ComponentBase<StatusComp>
    {
        StatusComp(bool isActive = true, StateType currentState = StateType{BuiltInStates::Idle})
            : ComponentBase::ComponentBase(Name), is_Active(isActive), m_CurrentState(currentState) {}

        static constexpr const char* Name = "Status";

		// data members
		std::unordered_map<ActionType, StateType> m_Actions;    // [key, value] = [action, state]


        bool is_Active;
        StateType m_CurrentState; // Idle, Moving, Attacking, etc. This maps to AnimationMap
        
        void OnReflect() override;
    };

    inline void to_json(json& j, const StatusComp& comp)
    {
        j["m_Actions"] = json::array();
        for (const auto& [action, state] : comp.m_Actions)
        {
            j["m_Actions"].push_back(
                {
                    {"Action", action},
                    {"State", state}
                });
        }
        j["is_Active"] = comp.is_Active;
    }

    inline void from_json(const json& j, StatusComp& comp)
    {
        comp.m_Actions.clear();
        

        if (j.contains("m_Actions") && j["m_Actions"].is_array())
        {
            for (const auto& item : j["m_Actions"])
            {
                ActionType action = item.at("Action").get<ActionType>();
                StateType state = item.at("State").get<StateType>();
                comp.m_Actions[action] = state;
            }
        }
        comp.is_Active = j.at("is_Active").get<bool>();
		comp.m_CurrentState = StateType{ BuiltInStates::Idle }; // Default to Idle on load
    }
}