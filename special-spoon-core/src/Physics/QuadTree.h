#pragma once

#include "SFML/Graphics.hpp"
#include <set>

namespace Spoon
{
    // class Node;
    class Scene;

    struct QT_GridNode
    {
        sf::FloatRect body;
        // std::vector<Node*> collision_buffer;
        sf::RectangleShape rect;
    };

    class Quadtree
    {
    public:
        Quadtree() {}
        ~Quadtree() {}

        void BuildTree(sf::FloatRect bounds);
        // void Populate(Scene& sceneroot);
        // void Insert(Node* node);
        // std::set<std::pair<Node*, Node*>> GeneratePairs();
        // void ProcessCollisionBuffer();

        // std::vector<QT_GridNode>& GetNodes() { return m_GridNodes; }

    private:
        std::vector<QT_GridNode> m_GridNodes{8};

    };
}