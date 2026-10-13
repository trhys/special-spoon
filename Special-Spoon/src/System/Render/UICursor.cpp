#include "UICursor.h"
#include "ECS/Components/Render/UI/UIComp.h"

namespace Spoon
{
    void UICursor::UpdateHover(UIComp* comp)
    {
        if (!comp)
            return;
        if (comp->isHovered)
            hoveredComp = comp;
    }

    void UICursor::UpdateFocus(UIComp* comp)
    {
        if (!comp)
            return;
        if (comp->isFocused)
            focusedComp = comp;
    }

    void UICursor::UpdatePressed(UIComp* comp)
    {
        if (!comp)
            return;
    }
}