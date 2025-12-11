#pragma once

#include "Core.h"
#include <functional>
#include <unordered_map>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Spoon
{
    class EntityManager;
    struct UUID;
    
    using Loader = std::function<void(EntityManager& manager, UUID id, const json& comp)>;

    class SPOON_API ComponentLoaders
    {
    public:
        ComponentLoaders() {}

        static std::unordered_map<std::string, Loader>& GetCompLoaders()
        {
            static std::unordered_map<std::string, Loader> s_CompLoaders;
            return s_CompLoaders;
        }

        static void RegisterCompLoader(const std::string& type, Loader loader)
        {
            GetCompLoaders()[type] = loader;
        }

    };
}