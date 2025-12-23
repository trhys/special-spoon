#pragma once

#include "System/System.h"
#include "QuadTree.h"
#include "Core/EntityManager.h"

namespace Spoon
{
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem() : ISystem::ISystem("Physics") {}
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            // Physics update logic goes here
        }

        void OnInit()
        {
            // Get config and build tree
        }

    private:
        Quadtree quadtree;
    };
}