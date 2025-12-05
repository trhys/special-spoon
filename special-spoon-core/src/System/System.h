#pragma once

#include "Core/Core.h"
#include "Core/EntityManager.h"
#include "SFML/System/Time.hpp"

namespace Spoon
{
    class ISystem
    {
    public:
        ISystem() {}
        virtual ~ISystem() {}

        virtual void Update(sf::Time tick, EntityManager& manager) = 0;
    };
}