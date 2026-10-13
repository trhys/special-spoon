#pragma once

#include "ECS/Components/Component.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
    // need this here to inform rendering on whether
    // this component is in world space or screen space (see WorldProjection.h)
    enum class UISpace
    {
        World,
        Screen
    };

    struct UIComp : public ComponentBase<UIComp>
    {
        UIComp() : ComponentBase(Name) {}

        static constexpr const char* Name = "UI";

        // data members
        bool active = false;
        bool isFocused = false;
        bool isHovered = false;
        UISpace space = UISpace::Screen;

        ActionType emittedAction = ActionType{BuiltInActions::None};

        // methods
        bool HitTest(sf::Vector2f mousePosition, const sf::FloatRect* spriteBounds, const sf::FloatRect* textBounds);
        void ClearActiveState(bool* spriteState, bool* textState);
        void SetActiveState(bool* spriteState, bool* textState);
        void SetFocused(bool focused) { isFocused = focused; }
        bool IsScreenSpace() const { return space == UISpace::Screen; }
        bool IsWorldSpace() const { return space == UISpace::World; }
        void OnReflect() override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UIComp, active, space, emittedAction)
}