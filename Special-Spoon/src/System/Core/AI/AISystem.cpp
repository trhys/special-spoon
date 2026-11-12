#include "AISystem.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "Utils/Macros.h"

#include <type_traits>

namespace Spoon
{
    void AISystem::Update(sf::Time tick, EntityManager& manager)
    {
        // prefetch action queue and arrays
        auto& actionQueue = Application::Get().GetActionQueue();
        auto& aiArray = manager.GetArray<AIComponent>(AIComponent::Name);

        // remove inactive runtimes
        CullRuntimes(aiArray);

        for (auto& [id, index] : aiArray.m_IdToIndex)
        {
            // check and create script runtime if it doesn't exist
            if (m_ScriptRuntimes.find(id) == m_ScriptRuntimes.end())
            {
                ScriptRuntime runtime;
                runtime.env = sol::environment(m_Lua, sol::create, m_Lua.globals());
                runtime.blackboard = m_Lua.create_table();
                runtime.api = CreateInterface(manager, id);

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
                } else {
                    runtime.active = false;
                    const sol::error err = loaded;
                    SS_DEBUG_LOG("[AI SYSTEM] ID:" + id.ToString() + " Failed to load script: " + aiComp.m_ActiveScript.string() + " Error: " + err.what());
                }
                m_ScriptRuntimes[id] = runtime;
            }
            // execute the onUpdate function if it exists
            auto& runtime = m_ScriptRuntimes[id];
            if (!runtime.onUpdate.valid() || !runtime.active)
                continue;
            auto result = runtime.onUpdate(
                tick.asSeconds(),
                runtime.blackboard,
                runtime.api
            );
            if (!result.valid())
            {
                runtime.active = false;
                const sol::error err = result;
                SS_DEBUG_LOG("[AI SYSTEM] ID:" + id.ToString() + " Error executing on_update function -> " + err.what());
                SS_DEBUG_LOG("[AI SYSTEM] ID:" + id.ToString() + " Marking runtime as inactive due to error.");
            }
        }
    }

    sol::table AISystem::CreateInterface(EntityManager& manager, UUID id)
    {
        sol::table api = m_Lua.create_table();

        api.set_function("push_action", [id](const std::string& name)
        {
            const ActionType action = ActionRegistry::Get().GetAction(name);
            if (!action)
                throw std::runtime_error("AI Request Error: Invalid action name: " + name);
            Application::Get().GetActionQueue().CreateAndPush(id, action, 0);
        });

        // register all queries from the script API as functions in the Lua interface
        for (const auto& [name, query] : m_ScriptAPI.GetQueries())
        {
            // register a Lua function for the query that converts its result to a Lua object
            api.set_function(name, 
                [this, id, &manager, query]() -> sol::object {
                    return std::visit(
                        [this](const auto& value) -> sol::object {
                            using T = std::decay_t<decltype(value)>;
                            if constexpr (std::is_same_v<T, sf::Vector2f>)
                            {
                                sol::table vecTable = m_Lua.create_table();
                                vecTable["x"] = value.x;
                                vecTable["y"] = value.y;
                                return sol::make_object(m_Lua, vecTable);
                            } else if constexpr (std::is_same_v<T, std::vector<UUID>>){
                                sol::table vecTable = m_Lua.create_table();
                                int index = 1;
                                for (const auto& uuid : value)
                                {
                                    vecTable[index++] = uuid.ToString();
                                }
                                return sol::make_object(m_Lua, vecTable);
                            } else{
                                return sol::make_object(m_Lua, value);
                            }
                        },
                        query(manager, id)
                    );
                });
        }

        return api;
    }

    void AISystem::ResetRuntimes()
    {
        m_ScriptRuntimes.clear();
    }

    void AISystem::CullRuntimes(const ComponentArray<AIComponent>& aiArray)
    {
        // remove runtimes for entities that no longer exist in the AI array
        for (auto it = m_ScriptRuntimes.begin(); it != m_ScriptRuntimes.end(); )
        {
            if (aiArray.m_IdToIndex.find(it->first) == aiArray.m_IdToIndex.end())
                it = m_ScriptRuntimes.erase(it);
            else
                ++it;
        }
    }
}