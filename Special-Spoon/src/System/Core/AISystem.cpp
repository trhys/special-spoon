#include "AISystem.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "Utils/Macros.h"

namespace Spoon
{
    void AISystem::Update(sf::Time tick, EntityManager& manager)
    {
        // prefetch action queue and arrays
        auto& actionQueue = Application::Get().GetActionQueue();
        auto& aiArray = manager.GetArray<AIComponent>(AIComponent::Name);

        for (auto& [id, index] : aiArray.m_IdToIndex)
        {
            // check and create script runtime if it doesn't exist
            if (m_ScriptRuntimes.find(id) == m_ScriptRuntimes.end())
            {
                ScriptRuntime runtime;
                runtime.env = sol::environment(m_Lua, sol::create, m_Lua.globals());
                runtime.blackboard = m_Lua.create_table();
                runtime.api = CreateInterface(id);

                // load the script file into the runtime's onUpdate function
                AIComponent& aiComp = aiArray.m_Components[index]; 
                auto loaded = m_Lua.safe_script_file(
                    aiComp.m_ActiveScript.string(), 
                    runtime.env,
                    sol::script_pass_on_error
                );
                if (loaded.valid())
                {
                    // verify on_update function exists
                    if (runtime.env["on_update"].valid())
                        runtime.onUpdate = runtime.env["on_update"];
                    else
                        SS_DEBUG_LOG("[AI SYSTEM] ID:" + id.ToString() + " Missing on_update function in script: " + aiComp.m_ActiveScript.string());
                }
                m_ScriptRuntimes[id] = runtime;
            }
            // execute the onUpdate function if it exists
            auto& runtime = m_ScriptRuntimes[id];
            if (!runtime.onUpdate.valid())
                continue;
            auto result = runtime.onUpdate(
                tick.asSeconds(),
                runtime.blackboard,
                runtime.api
            );
        }
    }

    sol::table AISystem::CreateInterface(UUID id)
    {
        sol::table api = m_Lua.create_table();

        api.set_function("push_action", [id](const std::string& name)
        {
            const ActionType action = ActionRegistry::Get().GetAction(name);
            if (!action)
                throw std::runtime_error("AI Request Error: Invalid action name: " + name);
            Application::Get().GetActionQueue().CreateAndPush(id, action, 0);
        });

        return api;
    }

    void AISystem::ResetRuntimes()
    {
        m_ScriptRuntimes.clear();
    }
}