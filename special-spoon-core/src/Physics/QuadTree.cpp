#include "QuadTree.h"
#include "Scene/Scene.h"
#include <optional>

namespace Spoon
{
    void Quadtree::BuildTree(sf::FloatRect bounds)
    {
        // Split the scene bounds into 8 equal grid squares
        sf::Vector2f node_size = {bounds.size.x/4, bounds.size.y/2};
        for(auto& leaf : m_GridNodes)
        {
            leaf.body.size = node_size;
        }
        for(auto it = m_GridNodes.begin(); it < m_GridNodes.end(); it++)
        {
            if(it < m_GridNodes[3])
            {
                float top = static_cast<float>*it
                m_GridNodes[it].position.x = node_size.x * top;
                m_GridNodes[it].position.y = 0;
            }
            else 
            {
                float bottom = static_cast<float>*it - 4.0;
                m_GridNodes[it].position.x = node_size.x * bottom;
                m_GridNodes[it].position.y = node_size.y;
            }
        }
    }

    void CollisionDetector::GetCollisionBodies(const Scene& sceneroot)
    {
        for(auto& leaf : m_GridNodes)
        {
            for(auto& child : sceneroot.GetChildren())
            {
                if(const std::optional intersect = leaf.body.findIntersection(child.GetBoundingBox()))
                {
                    leaf.collision_buffer.push_back(child);
                }
            }
        }
    }

    void CollisionDetector::ProcessCollisionBuffer() // TODO : Collision handling needs some kind of callback to help respond (where collision occured for example)
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
                        SS_COLLISION_TEST
                        leaf.collision_buffer[a]->CollisionDetected();
                        leaf.collision_buffer[b]->CollisionDetected();
                    }
                }
            }
        }
        collision_buffer.clear();
    }
}