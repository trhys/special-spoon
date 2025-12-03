#include "QuadTree.h"
#include "Scene/Scene.h"
#include <ECS/PhysComp.h>

#include <optional>
#include <iterator>
#include <set>
#include <algorithm>


namespace Spoon
{
    void Quadtree::BuildTree(sf::FloatRect bounds)
    {
        // Split the scene bounds into 8 equal grid squares
        sf::Vector2f node_size = {bounds.size.x/4, bounds.size.y/2};
        for(auto& leaf : m_GridNodes)
        {
            leaf.body.size = node_size;

            // Show the quadtree nodes on-screen for visual debugging
            leaf.rect.setSize(node_size);
            leaf.rect.setOutlineThickness(10.0f);
            leaf.rect.setFillColor(sf::Color::Transparent);
        }
        for(size_t in = 0; in < m_GridNodes.size(); in++)
        {
            if(in < 4)
            {
                m_GridNodes[in].body.position.x = node_size.x * in;
                m_GridNodes[in].body.position.y = 0;
                m_GridNodes[in].rect.setPosition({ node_size.x * in, 0 });
            }
            else 
            {
                int bottom_index = in - 4;
                m_GridNodes[in].body.position.x = node_size.x * bottom_index;
                m_GridNodes[in].body.position.y = node_size.y;
                m_GridNodes[in].rect.setPosition({ node_size.x * bottom_index, node_size.y });
            }
        }
    }

    void Quadtree::Populate(Scene& sceneroot)
    {
        for(auto& leaf : m_GridNodes) { leaf.collision_buffer.clear(); }
        for(auto& child : sceneroot.GetCollidablesGraph()) { Insert(child); }
    }

    void Quadtree::Insert(Node* node)
    {
        for(auto& leaf : m_GridNodes)
        {
            if(const std::optional intersect = leaf.body.findIntersection(node->GetComponent<PhysComp>()->GetCollisionBox()))
                {
                    leaf.collision_buffer.push_back(node);
                }
        }
    }

    std::set<std::pair<Node*, Node*>> Quadtree::GeneratePairs()
    {
        std::set<std::pair<Node*, Node*>> unique_pairs;
        for(auto& leaf : m_GridNodes)
        {
            for(auto a = 0; a < leaf.collision_buffer.size(); a++)
            {
                for(auto b = a + 1; b < leaf.collision_buffer.size(); b++)
                {
                    Node* objA = leaf.collision_buffer[a];
                    Node* objB = leaf.collision_buffer[b];

                    if(objA > objB) { std::swap(objA, objB); }

                    unique_pairs.insert({objA, objB});
                }
            }
        }
        return unique_pairs;
    }

    void Quadtree::ProcessCollisionBuffer() // TODO : Collision handling needs some kind of callback to help respond (where collision occured for example)
    {
        std::set<std::pair<Node*, Node*>> unique_pairs = GeneratePairs();
        for(auto& pair : unique_pairs)
        {
            Node* objA = pair.first;
            Node* objB = pair.second;
            if(const std::optional collision = objA->GetComponent<PhysComp>()->GetCollisionBox().findIntersection(objB->GetComponent<PhysComp>()->GetCollisionBox()))
            {
                objA->CollisionDetected();
                objB->CollisionDetected();
            }
        }
    }

    
}