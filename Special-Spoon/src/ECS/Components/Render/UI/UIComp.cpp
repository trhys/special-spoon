#include "UIComp.h"
#include "Editor/Utils/Tooltips.h"

namespace Spoon
{
    bool UIComp::HitTest(sf::Vector2f mousePosition, const sf::FloatRect* spriteBounds, const sf::FloatRect* textBounds)
    {
        if (!active)
            return false;

        // check that mousePosition is inside the UI element's bounds
        if ((spriteBounds && spriteBounds->contains(mousePosition)) || (textBounds && textBounds->contains(mousePosition)))
        {
            return true;
        }
        return false;
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

        ImGui::TextDisabled("Cursor Status");
        ImGui::TextDisabled(isFocused ? "Focused" : "Not Focused");
        ImGui::TextDisabled(isHovered ? "Hovered" : "Not Hovered");
        
        ImGui::TextDisabled("Select projection type for the UI element");
        if (ImGui::BeginCombo("Projection Space", space == UISpace::World ? "World Space" : "Screen Space"))
        {
            if (ImGui::Selectable("World Space", space == UISpace::World))
                space = UISpace::World;
            if (ImGui::Selectable("Screen Space", space == UISpace::Screen))
                space = UISpace::Screen;
            ImGui::EndCombo();
        } ImGui::SameLine();
        HelpMarker("Projection space is the coordinate space in which the UI element is rendered.\n"
            "Screen represents an overlay/hud, where world is rendered in the game world.");
        AvailableActionsList(emittedAction.m_ID, "Emitted Action");
    }
}