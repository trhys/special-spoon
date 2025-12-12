#pragma once

#include "Core/Core.h"
#include "nlohmann/json.hpp"

#include <memory>
#include <functional>
#include <unordered_map>

using json = nlohmann::json;

namespace Spoon
{
    class ISystem;

    class SPOON_API SystemLoaders
    {
    public:
        SystemLoaders() {}
        ~SystemLoaders() {}

        using Loader = std::function<std::unique_ptr<ISystem>(const json& systemData)>;

        static std::unordered_map<std::string, Loader>& GetSysLoaders()
        {
            static std::unordered_map<std::string, Loader> s_SysLoaders;
            return s_SysLoaders;
        }

        static void RegisterSysLoader(const std::string& type, Loader loader)
        {
            SS_DEBUG_LOG("[SYSTEM] Registering loader for system: " + type)
            GetSysLoaders()[type] = loader;
        }
    };
}