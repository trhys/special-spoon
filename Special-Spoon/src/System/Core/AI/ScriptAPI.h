#pragma once 

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
        sf::Vector2f
    >;

    class EntityManager;
    class ScriptAPI
    {
    public:
        ScriptAPI() = default;
        ~ScriptAPI() = default;

        using Query = std::function<QueryResult(EntityManager&, UUID)>;

        void RegisterQuery(const std::string& name, Query query);
        const std::unordered_map<std::string, Query>& GetQueries() const;

    private:
        std::unordered_map<std::string, Query> m_Queries;
    };
}