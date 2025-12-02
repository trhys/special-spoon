#pragma once

#include "Core.h"

namespace Spoon
{
    class Component
    {
    public:
        Component() {}
        virtual ~Component() {}

        virtual void Update(sf::Time tick) {}
    };
}