#pragma once

#include "Core/Core.h"
#include "Utils/Macros.h"

#include <functional>
#include <unordered_map>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Spoon
{
    class EntityManager;
    struct UUID;
    
    using Loader = std::function<void(EntityManager& manager, UUID id, const json& comp)>;

    class SPOON_API ComponentRegistry
    {
    public:
        static ComponentRegistry& Get()
        {
            static ComponentRegistry instance;
            return instance;
        }

        void RegisterLoader(const std::string& type, Loader loader)
        {
            SS_DEBUG_LOG("[COMPONENT] Registering component loader for type: " + type)
            m_Loaders[type] = loader;
        }

        const std::unordered_map<std::string, Loader>& GetLoaders() const
        {
            return m_Loaders;
        }

        // Delete copy/move to enforce singleton
        ComponentRegistry(const ComponentRegistry&) = delete;
        ComponentRegistry& operator=(const ComponentRegistry&) = delete;

    private:
        ComponentRegistry() = default;
        ~ComponentRegistry() = default;

        std::unordered_map<std::string, Loader> m_Loaders;
    };
}
