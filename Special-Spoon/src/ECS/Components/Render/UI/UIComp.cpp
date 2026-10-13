#include "UIComp.h"
#include "Editor/Utils/Tooltips.h"

namespace Spoon
{
    void UIComp::HitTest(sf::Vector2f mousePosition, const sf::FloatRect* spriteBounds, const sf::FloatRect* textBounds)
    {
        isHovered = false;
        if (!active)
            return;

        // check that mousePosition is inside the UI element's bounds
        if ((spriteBounds && spriteBounds->contains(mousePosition)) || (textBounds && textBounds->contains(mousePosition)))
        {
            isHovered = true;
        }

    }

    void UIComp::ClearActiveState(bool* spriteState, bool* textState)
    {
        if (spriteState)
            *spriteState = false;
        if (textState)
            *textState = false;
        active = false;
    }

    void UIComp::SetActiveState(bool* spriteState, bool* textState)
    {
        if (spriteState)
            *spriteState = true;
        if (textState)
            *textState = true;
        active = true;
    }

    void UIComp::OnReflect()
    {
        ImGui::Checkbox("Active", &active);
        ImGui::Checkbox("Focused", &isFocused);
        ImGui::Checkbox("Hovered", &isHovered);

        AvailableActionsList(emittedAction.m_ID, "Emitted Action");
    }
}