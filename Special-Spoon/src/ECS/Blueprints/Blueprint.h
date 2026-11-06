#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace Spoon
{
    struct ComponentBlueprint
    {
        std::string m_Name;
        json m_Data;
    };

    struct EntityBlueprint
    {
        std::string m_Name;
        std::vector<ComponentBlueprint> m_Components;
    };

    EntityBlueprint ResolveBlueprint(const std::filesystem::path& path);

    inline void to_json(json& j, const ComponentBlueprint& comp)
    {
        j = json{
            {"m_Name", comp.m_Name},
            {"m_Data", comp.m_Data}
        };
    }

    inline void from_json(const json& j, ComponentBlueprint& comp)
    {
        comp.m_Name = j.at("m_Name").get<std::string>();
        comp.m_Data = j.at("m_Data");
    }

    inline void from_json(const json& j, EntityBlueprint& entity)
    {
        entity = EntityBlueprint{
            j.at("m_Name").get<std::string>(),
            j.at("m_Components").get<std::vector<ComponentBlueprint>>()
        };
    }
    
    inline void to_json(json& j, const EntityBlueprint& entity)
    {
        j = json{
            {"m_Name", entity.m_Name},
            {"m_Components", entity.m_Components}
        };
    }
}
