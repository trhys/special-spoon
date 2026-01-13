#pragma once

#include "Core/Core.h"
#include "Utils/Macros.h"

#include "nlohmann/json.hpp"

#include <memory>
#include <functional>
#include <unordered_map>

using json = nlohmann::json;

namespace Spoon
{
    class ISystem;

    class SPOON_API SystemRegistry
    {
    public:
        using Loader = std::function<std::unique_ptr<ISystem>(const json* systemData)>;

        static SystemRegistry& Get()
        {
            static SystemRegistry instance;
            return instance;
        }

        void RegisterLoader(const std::string& type, Loader loader)
        {
            SS_DEBUG_LOG("[SYSTEM] Registering loader for system: " + type)
            m_Loaders[type] = loader;
        }

        const std::unordered_map<std::string, Loader>& GetLoaders() const
        {
            return m_Loaders;
        }

        // Delete copy/move to enforce singleton
        SystemRegistry(const SystemRegistry&) = delete;
        SystemRegistry& operator=(const SystemRegistry&) = delete;

    private:
        SystemRegistry() = default;
        ~SystemRegistry() = default;

        std::unordered_map<std::string, Loader> m_Loaders;
    };
}
