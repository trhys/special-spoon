#pragma once

namespace Spoon
{
    struct UIComp;
    
    struct UICursor {
        UIComp* hoveredComp = nullptr;
        UIComp* focusedComp = nullptr;
        UIComp* pressedComp = nullptr;

        void UpdateHover(UIComp* comp);
        void UpdateFocus(UIComp* comp);
        void UpdatePressed(UIComp* comp);
    };
}