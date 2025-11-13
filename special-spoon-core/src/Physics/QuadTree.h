#pragma once

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include <vector>

namespace Spoon
{
    class Node;
    class Scene;

    struct QT_GridNode
    {
        sf::FloatRect body;
        std::vector<Node*> collision_buffer;
    };

    class Quadtree
    {
    public:
        Quadtree() {}
        ~Quadtree() {}

        void BuildTree(sf::FloatRect bounds);
        void GetCollisionBodies(const Scene& sceneroot);
        void ProcessCollisionBuffer();

    private:
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}