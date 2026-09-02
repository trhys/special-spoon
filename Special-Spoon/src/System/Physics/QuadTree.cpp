#include "QuadTree.h"
#include "Core/EntityManager/EntityManager.h"

#include <optional>
#include <iterator>
#include <set>
#include <algorithm>


namespace Spoon
{

    // helper for processbuffer
    sf::Vector2f ComputeSeparation(const sf::FloatRect& boxA, const sf::FloatRect& boxB)
    {
        // Find overlap on each axis
        float overlapLeft = (boxA.left + boxA.width) - boxB.left;
        float overlapRight = boxB.left + boxB.width - boxA.left;
        float overlapTop = (boxA.top + boxA.height) - boxB.top;
        float overlapBottom = boxB.top + boxB.height - boxA.top;
        
        // Find minimum overlap (axis of least penetration)
        float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
        
        // Return separation direction based on minimum overlap
        if(minOverlap == overlapLeft)
            return {-overlapLeft / 2.0f, 0.0f};  // A moves left, B moves right
        else if(minOverlap == overlapRight)
            return {overlapRight / 2.0f, 0.0f};  // A moves right, B moves left
        else if(minOverlap == overlapTop)
            return {0.0f, -overlapTop / 2.0f};   // A moves up, B moves down
        else
            return {0.0f, overlapBottom / 2.0f}; // A moves down, B moves up
    }

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
        for(auto& entity : manager.GetAllEntitiesWithComponent<PhysicsComp>(PhysicsComp::Name))
        {
            PhysicsComp& phys = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
            sf::FloatRect entityBox = phys.GetCollisionBox();
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
            PhysicsComp& physA = manager.GetComponent<PhysicsComp>(entityA, PhysicsComp::Name);
            PhysicsComp& physB = manager.GetComponent<PhysicsComp>(entityB, PhysicsComp::Name);

            if(const std::optional collision = physA.GetCollisionBox().findIntersection(physB.GetCollisionBox()))
            {
                physA.CollisionDetected();
                physB.CollisionDetected();

                sf::Vector2f separation = ComputeSeparation(physA.GetCollisionBox(), physB.GetCollisionBox());
            
                if(!physA.isStatic)
                {
                    physA.m_CollisionBox.position += separation;
                }
                if(!physB.isStatic)
                {
                    physB.m_CollisionBox.position -= separation;
                }
    
                // Step 2: Get MovementComp velocity (if exists) and apply impulse
                // Entity A movement
                if(manager.GetArray<MovementComp>(MovementComp::Name).m_IdToIndex.count(entityA))
                {
                    MovementComp& movA = manager.GetComponent<MovementComp>(entityA, MovementComp::Name);
                    
                    // Entity B movement (if exists)
                    if(manager.GetArray<MovementComp>(MovementComp::Name).m_IdToIndex.count(entityB))
                    {
                        MovementComp& movB = manager.GetComponent<MovementComp>(entityB, MovementComp::Name);
                        
                        // Impulse calculation
                        sf::Vector2f relativeVelocity = movA.velocity - movB.velocity;
                        sf::Vector2f collisionNormal = separation;
                        float length = std::sqrt(collisionNormal.x * collisionNormal.x + 
                                                 collisionNormal.y * collisionNormal.y);
                        if(length > 0.001f)
                        {
                            collisionNormal /= length;
                        }
                        
                        float velAlongNormal = relativeVelocity.x * collisionNormal.x + 
                                               relativeVelocity.y * collisionNormal.y;
                        
                        if(velAlongNormal < 0.0f)
                        {
                            float e = (physA.restitution + physB.restitution) / 2.0f;
                            
                            float invMassA = physA.isStatic ? 0.0f : 1.0f / physA.mass;
                            float invMassB = physB.isStatic ? 0.0f : 1.0f / physB.mass;
                            float totalInvMass = invMassA + invMassB;
                            
                            if(totalInvMass > 0.001f)
                            {
                                float j = -(1.0f + e) * velAlongNormal / totalInvMass;
                                sf::Vector2f impulse = collisionNormal * j;
                                
                                if(!physA.isStatic)
                                    movA.velocity += impulse * invMassA;
                                if(!physB.isStatic)
                                    movB.velocity -= impulse * invMassB;
                            }
                        }
                    }
                    // Entity B has no movement; only A gets impulse
                    else if(!physA.isStatic)
                    {
                        // Simple bounce off static/non-moving object
                        sf::Vector2f collisionNormal = separation;
                        float length = std::sqrt(collisionNormal.x * collisionNormal.x + 
                                                 collisionNormal.y * collisionNormal.y);
                        if(length > 0.001f)
                        {
                            collisionNormal /= length;
                            float e = physA.restitution;
                            movA.velocity = collisionNormal * (std::sqrt(movA.velocity.x * movA.velocity.x + 
                                                                         movA.velocity.y * movA.velocity.y) * e);
                        }
                    }
                }
            }
        }
    }   
}
