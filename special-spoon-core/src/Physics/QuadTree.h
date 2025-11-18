#pragma once

#ifdef SS_PHYS_TEST
    #include "SFML/Graphics.hpp"
#endif

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

        #ifdef SS_PHYS_TEST
            sf::RectangleShape rect;
        #endif
    };

    class Quadtree
    {
    public:
        Quadtree() {}
        ~Quadtree() {}

        void BuildTree(sf::FloatRect bounds);
        void GetCollisionBodies(const Scene& sceneroot);
        void ProcessCollisionBuffer();

        std::vector<QT_GridNode>* GetNodes() { return &m_GridNodes; }

    private:
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}