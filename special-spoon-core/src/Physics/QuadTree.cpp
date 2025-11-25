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

            // Show the quadtree nodes on-screen for visual debugging
            leaf.rect.setSize(node_size);
            leaf.rect.setOutlineThickness(10.0f);
            leaf.rect.setOutlineColor(sf::Color::Transparent);
        }
        for(size_t in = 0; in < m_GridNodes.size(); in++)
        {
            if(in < 4)
            {
                m_GridNodes[in].body.position.x = node_size.x * in;
                m_GridNodes[in].body.position.y = 0;
                m_GridNodes[in].rect.setPosition(body.position);
            }
            else 
            {
                int bottom_index = in - 4;
                m_GridNodes[in].body.position.x = node_size.x * bottom_index;
                m_GridNodes[in].body.position.y = node_size.y;
                m_GridNodes[in].rect.setPosition(body.position);
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