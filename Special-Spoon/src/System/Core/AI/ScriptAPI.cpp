#include "ScriptAPI.h"
#include "Utils/Macros.h"

namespace Spoon
{
    void ScriptAPI::RegisterQuery(const std::string& name, Query query)
    {
        // validate and register the query
        if (!name.empty() && query)
        {
            if (m_Queries.find(name) == m_Queries.end())
            {
                m_Queries[name] = query;
            } else {
                SS_DEBUG_LOG("[ScriptAPI] Query with name '" + name + "' already exists.")
            }
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