#pragma once

#include "System/System.h"
#include "System/Core/AI/ScriptAPI.h"
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
        AISystem(ScriptAPI scriptAPI = ScriptAPI()) : ISystem::ISystem(Name), m_ScriptAPI(std::move(scriptAPI)) {
            m_Lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
        }
        static constexpr const char* Name = "AI";

        void Update(sf::Time tick, EntityManager& manager) override;
        sol::table CreateInterface(UUID id);
        void ResetRuntimes();
    private:
        sol::state m_Lua;
        std::unordered_map<UUID, ScriptRuntime> m_ScriptRuntimes;
        ScriptAPI m_ScriptAPI;
    };
}