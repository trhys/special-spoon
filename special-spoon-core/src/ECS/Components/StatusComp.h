#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    struct StatusComp : public Component
    {
        bool is_Active;
        bool is_Visible;
    };
}