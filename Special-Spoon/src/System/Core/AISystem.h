#pragma once

#include "System/System.h"
#include "sol/sol.hpp"

namespace Spoon
{
    class EntityManager;

    struct ScriptRuntime
    {
        sol::environment env;
        sol::table blackboard;
        sol::table api;
        sol::protected_function onUpdate;
    };

    class AISystem : public ISystem
    {
    public:
        AISystem() : ISystem::ISystem(Name) {
            m_Lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
        }
        static constexpr const char* Name = "AI";

        void Update(sf::Time tick, EntityManager& manager) override;
        sol::table CreateInterface(UUID id);
        void ResetRuntimes();
    private:
        sol::state m_Lua;
        std::unordered_map<UUID, ScriptRuntime> m_ScriptRuntimes;
    };
}