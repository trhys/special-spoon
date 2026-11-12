#pragma once 

#include "Core/Core.h"
#include "ECS/UUID.h"
#include "SFML/Graphics.hpp"

#include <variant>
#include <functional>
#include <unordered_map>

namespace Spoon
{
    using QueryResult = std::variant<
        bool,
        int, 
        float, 
        std::string,
        sf::Vector2f,
        std::vector<UUID>
    >;

    class EntityManager;
    class SPOON_API ScriptAPI
    {
    public:
        ScriptAPI() = default;
        ~ScriptAPI() = default;

        using Query = std::function<QueryResult(EntityManager&, UUID)>;
        using QueryTarget = std::function<QueryResult(EntityManager&, UUID, UUID)>;
        using RegisteredQuery = std::variant<
            Query,
            QueryTarget
        >;

        void RegisterQuery(const std::string& name, Query query);
        void RegisterQuery(const std::string& name, QueryTarget query);
        const std::unordered_map<std::string, RegisteredQuery>& GetQueries() const;

    private:
        std::unordered_map<std::string, RegisteredQuery> m_Queries;
    };
}