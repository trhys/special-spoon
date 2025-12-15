#pragma once

#include "System/System.h"
#include "QuadTree.h"
#include "Core/EntityManager.h"

namespace Spoon
{
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem(sf::Vector2f gridSize) { quadtree.BuildTree(gridSize); }
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            // Physics update logic goes here
        }
    private:
        Quadtree quadtree;
    };
}