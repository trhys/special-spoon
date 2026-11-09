#pragma once

#include "System/System.h"
#include "sol/sol.hpp"

namespace Spoon
{
    class EntityManager;

    class AISystem : public ISystem
    {
    public:
        AISystem() : ISystem::ISystem(Name) {
            m_Lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
        }
        static constexpr const char* Name = "AI";

        void Update(sf::Time tick, EntityManager& manager) override;

    private:
        sol::state m_Lua;
    };
}