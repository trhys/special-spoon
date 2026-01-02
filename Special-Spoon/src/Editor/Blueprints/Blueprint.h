#pragma once

#include "Core/Core.h"
#include "ECS/ECS.h"

namespace Spoon
{
    class Blueprint
    {
    public:
        Blueprint(std::string displayName, std::initializer_list<std::string> comps) : m_Name(displayName), m_Components(comps) {}
        const std::vector<std::string>& GetComps() const { return m_Components; }
        const std::string& GetDisplayName() const { return m_Name; }
    private:
        std::string m_Name;
        std::vector<std::string> m_Components;
    };

    static inline Blueprint None("None", {});
    static inline Blueprint SpriteBlueprint("Sprite", {"Transform", "Sprite", "RenderLayer"});
    static inline Blueprint TextBlueprint("Text", {"Transform", "Text", "RenderLayer"});

    static inline std::vector<const Blueprint*> GetBlueprints()
    {
        return {
            &None,
            &SpriteBlueprint,
            &TextBlueprint
        };
    }
}