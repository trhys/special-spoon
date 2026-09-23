#pragma once

#include "ECS/Components/ColliderComp.h"
#include "SFML/Graphics.hpp"
#include <set>
#include <unordered_map>

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
        void Populate(EntityManager& manager, const std::unordered_map<UUID, sf::FloatRect>* boundsOverride = nullptr);
        std::set<std::pair<UUID, UUID>> GeneratePairs();

        // std::vector<QT_GridNode>& GetNodes() { return m_GridNodes; }

    private:
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}