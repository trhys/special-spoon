#pragma once

#include "Component.h"

namespace Spoon
{
    struct StatusComp : public Component
    {
        bool is_Active;
        bool is_Visible;
    };
}