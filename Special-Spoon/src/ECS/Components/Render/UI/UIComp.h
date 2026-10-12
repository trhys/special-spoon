#pragma once

#include "ECS/Components/Component.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
    struct UIComp : public ComponentBase<UIComp>
    {
        UIComp() : ComponentBase(Name) {}

        static constexpr const char* Name = "UI";

        // data members
        bool active = false;
        bool isFocused = false;
        bool isHovered = false;
        ActionType emittedAction = ActionType{BuiltInActions::None};

        // methods
        void OnReflect() override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UIComp, active, isFocused, isHovered, emittedAction)
}