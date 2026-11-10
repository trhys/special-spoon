#include "ScriptAPI.h"
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

    const std::unordered_map<std::string, ScriptAPI::Query>& ScriptAPI::GetQueries() const
    {
        return m_Queries;
    }

    // register core queries with the script API -- called from Application
    void RegisterCoreQueries(ScriptAPI& scriptAPI)
    {
        // Example core query registration
        scriptAPI.RegisterQuery("CoreQuery", [](EntityManager& manager, UUID id) {
            // query logic here
            return true;
        });
    }
}