#pragma once

#include "Core.h"
#include "SFML/Graphics.hpp"

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