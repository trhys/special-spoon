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
            
        }
    };
}