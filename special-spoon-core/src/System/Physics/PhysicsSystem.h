#pragma once

#include "System.h"
#include "QuadTree.h"

namespace Spoon
{
    class PhysicsSystem
    {
    public:
        PhysicsSystem() {}
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            // Physics update logic goes here
        }
        
    };
}