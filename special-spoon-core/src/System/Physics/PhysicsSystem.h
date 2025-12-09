#pragma once

#include "System.h"
#include "QuadTree.h"
#include "Core/EntityManager.h"

namespace Spoon
{
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem(sf::FloatRect scene_bounds) { quadtree.BuildTree(scene_bounds); }
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            // Physics update logic goes here
        }
    private:
        Quadtree quadtree;
    };
}