#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct StatusComp : public Component
    {
        StatusComp(bool isActive, std::string currentState)
            : is_Active(isActive), m_CurrentState(currentState) 
        {
        }

        bool is_Active;
        std::string m_CurrentState; // Idle, Moving, Attacking, etc. This maps to AnimationMap
    };
}