#pragma once

#include "Component.h"

namespace Spoon
{
    struct StateActionComp : public Component
    {
        StateActionComp(std::unordered_map<std::string, std::string> stateActions) : m_Actions(stateActions) {}

        std::unordered_map<std::string, std::string> m_Actions;
    };
}