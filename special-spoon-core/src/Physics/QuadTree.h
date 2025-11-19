#pragma once


#include "SFML/Graphics.hpp"


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
        sf::RectangleShape rect;
    };

    class Quadtree
    {
    public:
        Quadtree() {}
        ~Quadtree() {}

        void BuildTree(sf::FloatRect bounds);
        void GetCollisionBodies(Scene& sceneroot);
        void ProcessCollisionBuffer();

        std::vector<QT_GridNode>* GetNodes() { return &m_GridNodes; }

    private:
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}