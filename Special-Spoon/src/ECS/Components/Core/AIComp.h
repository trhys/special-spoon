#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct AIComponent : public ComponentBase<AIComponent>
    {
        AIComponent() : ComponentBase::ComponentBase(Name) {}
        static constexpr const char* Name = "AI Controller";

        void OnReflect() override;

        // for now - just a single script
        // todo: support multiple scripts and behavior trees
        std::filesystem::path m_ActiveScript; 
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AIComponent, m_ActiveScript)
}