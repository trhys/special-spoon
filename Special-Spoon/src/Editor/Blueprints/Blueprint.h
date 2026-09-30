#pragma once

#include "Core/Core.h"
#include "ECS/ECS.h"

namespace Spoon
{
    class Blueprint
    {
    public:
        Blueprint() = default;
        Blueprint(std::string displayName, std::vector<std::string> comps) : m_Name(displayName), m_Components(comps) {}
        const std::vector<std::string>& GetComps() const { return m_Components; }
        const std::string& GetDisplayName() const { return m_Name; }

        void SetName(const std::string& name) { m_Name = name; }
        void SetComponents(std::vector<std::string> comps) { m_Components = comps; }

        friend bool operator==(const Blueprint& lhs, const Blueprint& rhs) { return lhs.m_Name == rhs.m_Name; }
    private:
        std::string m_Name;
        std::vector<std::string> m_Components;
    };

    inline void to_json(json& j, const Blueprint& blueprint)
    {
        j = json{
            {"m_Name", blueprint.GetDisplayName()},
            {"m_Components", blueprint.GetComps()}
        };
    }

    inline void from_json(const json& j, Blueprint& blueprint)
    {
        blueprint = Blueprint(
            j.at("m_Name").get<std::string>(),
            j.at("m_Components").get<std::vector<std::string>>()
        );
    }

    static inline Blueprint None("None", {});
    static inline Blueprint SpriteBlueprint("Sprite", std::vector<std::string>{"Transform", "Sprite", "RenderLayer"});
    static inline Blueprint TextBlueprint("Text", std::vector<std::string>{"Transform", "Text", "RenderLayer"});

    static inline std::vector<Blueprint> GetDefaultBlueprints()
    {
        return {
            None,
            SpriteBlueprint,
            TextBlueprint
        };
    }

}
