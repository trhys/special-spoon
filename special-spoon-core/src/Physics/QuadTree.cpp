#include "QuadTree.h"
#include "Scene/Scene.h"

#include <optional>
#include <iterator>

namespace Spoon
{
    void Quadtree::BuildTree(sf::FloatRect bounds)
    {
        // Split the scene bounds into 8 equal grid squares
        sf::Vector2f node_size = {bounds.size.x/4, bounds.size.y/2};
        for(auto& leaf : m_GridNodes)
        {
            leaf.body.size = node_size;

            // TEST
            // leaf.rect.setSize(node_size);
            // leaf.rect.setOutlineThickness(10.0f);
            // leaf.rect.setOutlineColor(sf::Color(250, 100, 100));
            // TEST
        }
        for(auto it = m_GridNodes.begin(); it < m_GridNodes.end(); it++)
        {
            if(std::distance(m_GridNodes.begin(), it) < 4)
            {
                int top = std::distance(m_GridNodes.begin(), it);
                m_GridNodes[top].body.position.x = node_size.x * top;
                m_GridNodes[top].body.position.y = 0;
            }
            else 
            {
                int bottom = std::distance(m_GridNodes.begin(), it) - 4.0;
                m_GridNodes[bottom].body.position.x = node_size.x * bottom;
                m_GridNodes[bottom].body.position.y = node_size.y;
            }
        }
    }

    void Quadtree::GetCollisionBodies(Scene& sceneroot)
    {
        for(auto& leaf : m_GridNodes)
        {
            for(auto& child : sceneroot.GetChildren())
            {
                if(child->GetIsCollidable())
                {
                    if(const std::optional intersect = leaf.body.findIntersection(child->GetBoundingBox()))
                    {
                        leaf.collision_buffer.push_back(child);
                    }
                }
            }
        }
    }

    void Quadtree::ProcessCollisionBuffer() // TODO : Collision handling needs some kind of callback to help respond (where collision occured for example)
    {
        for(auto& leaf : m_GridNodes)
        {
            for(auto a = 0; a < leaf.collision_buffer.size(); a++)
            {
                for(auto b = a + 1; b < leaf.collision_buffer.size(); b++)
                {
                    if (const std::optional collision = leaf.collision_buffer[a]->GetBoundingBox().findIntersection(leaf.collision_buffer[b]->GetBoundingBox()))
                    {
                        // Handle collision
                        leaf.collision_buffer[a]->CollisionDetected();
                        leaf.collision_buffer[b]->CollisionDetected();
                    }
                }
            }
            leaf.collision_buffer.clear();
        }
    }
}