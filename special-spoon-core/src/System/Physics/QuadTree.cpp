#include "QuadTree.h"
#include "Core/EntityManager.h"

#include <optional>
#include <iterator>
#include <set>
#include <algorithm>


namespace Spoon
{
    void Quadtree::BuildTree(sf::Vector2f gridSize)
    {
        // Split the scene bounds into 8 equal grid squares
        sf::Vector2f node_size = {gridSize.x/4, gridSize.y/2};
        for(auto& leaf : m_GridNodes)
        {
            leaf.body.size = node_size;

            // Show the quadtree nodes on-screen for visual debugging
            leaf.rect.setSize(node_size);
            leaf.rect.setOutlineThickness(1.0f);
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

    void Quadtree::Populate(EntityManager& manager)
    {
        for(auto& leaf : m_GridNodes) { leaf.collision_buffer.clear(); }
        for(auto& entity : manager.GetAllEntitiesWithComponent<PhysicsComp>())
        {
            PhysicsComp* phys = manager.GetComponent<PhysicsComp>(entity);
            sf::FloatRect entityBox = phys->GetCollisionBox();
            for(auto& leaf : m_GridNodes)
            {
                if(const std::optional intersect = leaf.body.findIntersection(entityBox))
                {
                    leaf.collision_buffer.push_back(entity);
                }
            }
        }
    }

    std::set<std::pair<UUID, UUID>> Quadtree::GeneratePairs()
    {
        std::set<std::pair<UUID, UUID>> unique_pairs;
        for(auto& leaf : m_GridNodes)
        {
            for(auto a = 0; a < leaf.collision_buffer.size(); a++)
            {
                for(auto b = a + 1; b < leaf.collision_buffer.size(); b++)
                {
                    UUID entityA = leaf.collision_buffer[a];
                    UUID entityB = leaf.collision_buffer[b];

                    if(entityA > entityB) { std::swap(entityA, entityB); }

                    unique_pairs.insert({entityA, entityB});
                }
            }
        }
        return unique_pairs;
    }

    void Quadtree::ProcessCollisionBuffer(EntityManager& manager)
    {
        std::set<std::pair<UUID, UUID>> unique_pairs = GeneratePairs();
        for(auto& pair : unique_pairs)
        {
            UUID entityA = pair.first;
            UUID entityB = pair.second;
            PhysicsComp* physA = manager.GetComponent<PhysicsComp>(entityA);
            PhysicsComp* physB = manager.GetComponent<PhysicsComp>(entityB);

            if(const std::optional collision = physA->GetCollisionBox().findIntersection(physB->GetCollisionBox()))
            {
                physA->CollisionDetected();
                physB->CollisionDetected();
            }
        }
    }   
}