#pragma once

#include "nlohmann/json.hpp"

#include <memory>
#include <functional>
#include <unordered_map>

using json = nlohmann::json;

namespace Spoon
{
    class ISystem;

    class SystemLoaders
    {
    public:
        SystemLoaders() {}
        ~SystemLoaders() {}

        using Loader = std::function<std::unique_ptr<ISystem>(const json& systemData)>;

        static inline std::unordered_map<std::string, Loader> s_SysLoaders;

        static void RegisterSysLoader(const std::string& type, Loader loader)
        {
            s_SysLoaders[type] = loader;
        }
    };
}