#pragma once

#include <functional>
#include <unordered_map>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Spoon
{
    class EntityManager;
    struct UUID;
    
    using Loader = std::function<void(EntityManager& manager, UUID id, const json& comp)>;

    class ComponentLoader
    {
    public:
        ComponentLoader() {}
        ~ComponentLoader() {}

        static inline std::unordered_map<std::string, Loader> s_CompLoaders;

        static void RegisterCompLoader(const std::string& type, Loader loader)
        {
            s_CompLoaders[type] = loader;
        }
    };
}