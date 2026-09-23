#pragma once

#include "ECS/Components/ColliderComp.h"
#include "SFML/Graphics.hpp"
#include <set>
#include <vector>

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

        void BuildTree(sf::Vector2f gridSize);
        void Populate(EntityManager& manager);
        std::vector<UUID> Query(const sf::FloatRect& bounds) const;
        std::set<std::pair<UUID, UUID>> GeneratePairs();

        // std::vector<QT_GridNode>& GetNodes() { return m_GridNodes; }

    private:
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}