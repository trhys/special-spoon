#pragma once

#include "ECS/Components/ColliderComp.h"
#include "FrameMotion.h"
#include "SFML/Graphics.hpp"
#include <functional>
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
        using MotionProvider = std::function<FrameMotion(EntityManager&, UUID, float)>;

        Quadtree() {}
        ~Quadtree() {}

        void BuildTree(sf::Vector2f gridSize);
        void Populate(EntityManager& manager);
        std::vector<UUID> Query(const sf::FloatRect& bounds) const;
        std::set<std::pair<UUID, UUID>> GeneratePairs();

        // std::vector<QT_GridNode>& GetNodes() { return m_GridNodes; }

    private:
        void Populate(EntityManager& manager, float dt, bool useSweptBounds, const MotionProvider& motionProvider);
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}