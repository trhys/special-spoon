#pragma once

#include "Core/Core.h"
#include "Core/EntityManager/EntityManager.h"
#include "SFML/System/Time.hpp"

namespace Spoon
{
    class ISystem
    {
    public:
        ISystem(std::string name) : m_DisplayName(name) {}
        virtual ~ISystem() {}

        virtual void Update(sf::Time tick, EntityManager& manager) = 0;
        virtual void OnReflect() {}
        virtual json Serialize() { return nullptr; }

        std::string m_DisplayName;
        std::string& GetDisplayName() { return m_DisplayName; }
    };
}
