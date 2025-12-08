#pragma once

#include "ECS/Components/PhysicsComp.h"
#include "SFML/Graphics.hpp"
#include <set>

namespace Spoon
{
    class EntityManager;
    struct UUID;

    struct QT_GridNode
    {
        sf::FloatRect body;
        sf::RectangleShape rect;
        std::vector<UUID> collision_buffer;
    };

    class Quadtree
    {
    public:
        Quadtree() {}
        ~Quadtree() {}

        void BuildTree(sf::FloatRect bounds);
        void Populate(EntityManager& manager);
        std::set<std::pair<UUID, UUID>> GeneratePairs();
        void ProcessCollisionBuffer(EntityManager& manager);

        // std::vector<QT_GridNode>& GetNodes() { return m_GridNodes; }

    private:
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}