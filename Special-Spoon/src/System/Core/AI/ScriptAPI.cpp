#include "ScriptAPI.h"
#include "Core/EntityManager/EntityManager.h"
#include "Utils/Macros.h"

#include <algorithm>
#include <vector>

namespace Spoon
{
    namespace ReservedAPI {
        const std::vector<std::string> reserved = {"push_action"};
        bool IsReserved(const std::string& name)
        {
            return std::find(reserved.begin(), reserved.end(), name) != reserved.end();
        }
    }

    void ScriptAPI::RegisterQuery(const std::string& name, Query query)
    {
        // validate and register the query
        if (!name.empty() && query)
        {
            if (m_Queries.find(name) == m_Queries.end() && !ReservedAPI::IsReserved(name))
            {
                m_Queries[name] = query;
            } else {
                SS_DEBUG_LOG("[SCRIPT API] Query with name '" + name + "' already exists or is reserved.");
            }
        } else {
            SS_DEBUG_LOG("[SCRIPT API] Invalid query name or query is null.");
        }
    }

    void ScriptAPI::RegisterQuery(const std::string& name, QueryTarget query)
    {
        // validate and register the query
        if (!name.empty() && query)
        {
            if (m_Queries.find(name) == m_Queries.end() && !ReservedAPI::IsReserved(name))
            {
                m_Queries[name] = query;
            } else {
                SS_DEBUG_LOG("[SCRIPT API] Query with name '" + name + "' already exists or is reserved.");
            }
        } else {
            SS_DEBUG_LOG("[SCRIPT API] Invalid query name or query is null.");
        }
    }

    const std::unordered_map<std::string, ScriptAPI::RegisteredQuery>& ScriptAPI::GetQueries() const
    {
        return m_Queries;
    }

    // register core queries with the script API -- called from Application
    void RegisterCoreQueries(ScriptAPI& scriptAPI)
    {
        // transform based queries
        scriptAPI.RegisterQuery("get_position", [](EntityManager& manager, UUID id) {
            if (!manager.HasComponent<TransformComp>(id, TransformComp::Name))
                return sf::Vector2f{};
            auto& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
            return transform.GetPosition();
        });
        scriptAPI.RegisterQuery("get_target_position", [](EntityManager& manager, UUID id, UUID target) {
            if (!manager.HasComponent<TransformComp>(target, TransformComp::Name))
                return sf::Vector2f{};
            auto& transform = manager.GetComponent<TransformComp>(target, TransformComp::Name);
            return transform.GetPosition();
        });
        scriptAPI.RegisterQuery("get_facing", [](EntityManager& manager, UUID id) {
            if (!manager.HasComponent<TransformComp>(id, TransformComp::Name))
                return std::string{};
            auto& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
            return std::string(transform.GetFacingDirection());
        });

        // movement
        scriptAPI.RegisterQuery("get_velocity", [](EntityManager& manager, UUID id) {
            if (!manager.HasComponent<MovementComp>(id, MovementComp::Name))
                return sf::Vector2f{};
            auto& movement = manager.GetComponent<MovementComp>(id, MovementComp::Name);
            return movement.m_Velocity;
        });
    }
}