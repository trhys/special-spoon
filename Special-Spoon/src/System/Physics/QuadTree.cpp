#include "QuadTree.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/Components/MovementComp.h"
#include "ECS/Components/PhysicsComp.h"
#include "ECS/Components/TransformComp.h"

#include <algorithm>
#include <optional>

namespace Spoon
{
    namespace
    {
        sf::Vector2f GetFrameDelta(EntityManager& manager, UUID entity, float dt)
        {
            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            if (physicsArray.m_IdToIndex.count(entity))
            {
                auto& physics = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
                if (physics.bodyType == BodyType::Static)
                    return { 0.0f, 0.0f };
            }

            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            if (movementArray.m_IdToIndex.count(entity))
            {
                return manager.GetComponent<MovementComp>(entity, MovementComp::Name).m_ProposedDelta;
            }

            if (physicsArray.m_IdToIndex.count(entity))
            {
                auto& physics = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
                return physics.velocity * dt;
            }

            return { 0.0f, 0.0f };
        }

        sf::FloatRect ComputeSweptBounds(const sf::FloatRect& currentBounds, const sf::Vector2f& delta)
        {
            const sf::Vector2f startPosition = currentBounds.position - delta;
            const sf::Vector2f minPosition = {
                std::min(startPosition.x, currentBounds.position.x),
                std::min(startPosition.y, currentBounds.position.y)
            };
            const sf::Vector2f maxPosition = {
                std::max(startPosition.x + currentBounds.size.x, currentBounds.position.x + currentBounds.size.x),
                std::max(startPosition.y + currentBounds.size.y, currentBounds.position.y + currentBounds.size.y)
            };

            return sf::FloatRect(minPosition, maxPosition - minPosition);
        }
    }

    void Quadtree::BuildTree(sf::Vector2f gridSize)
    {
        sf::Vector2f node_size = { gridSize.x / 4, gridSize.y / 2 };
        for (auto& leaf : m_GridNodes)
        {
            leaf.body.size = node_size;
            leaf.rect.setSize(node_size);
            leaf.rect.setOutlineThickness(1.0f);
            leaf.rect.setFillColor(sf::Color::Transparent);
        }

        for (size_t in = 0; in < m_GridNodes.size(); in++)
        {
            if (in < 4)
            {
                m_GridNodes[in].body.position.x = node_size.x * static_cast<float>(in);
                m_GridNodes[in].body.position.y = 0.0f;
                m_GridNodes[in].rect.setPosition({ node_size.x * static_cast<float>(in), 0.0f });
            }
            else
            {
                int bottom_index = static_cast<int>(in) - 4;
                m_GridNodes[in].body.position.x = node_size.x * static_cast<float>(bottom_index);
                m_GridNodes[in].body.position.y = node_size.y;
                m_GridNodes[in].rect.setPosition({ node_size.x * static_cast<float>(bottom_index), node_size.y });
            }
        }
    }

    void Quadtree::Populate(EntityManager& manager, float dt, bool useSweptBounds)
    {
        for (auto& leaf : m_GridNodes)
            leaf.collision_buffer.clear();

        auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
        for (auto& entity : manager.GetAllEntitiesWithComponent<ColliderComp>(ColliderComp::Name))
        {
            if (!transformArray.m_IdToIndex.count(entity))
                continue;

            auto& transform = manager.GetComponent<TransformComp>(entity, TransformComp::Name);
            auto& collider = manager.GetComponent<ColliderComp>(entity, ColliderComp::Name);
            sf::FloatRect entityBox = collider.GetWorldBounds(transform.GetPosition());
            if (useSweptBounds)
            {
                entityBox = ComputeSweptBounds(entityBox, GetFrameDelta(manager, entity, dt));
            }
            for (auto& leaf : m_GridNodes)
            {
                if (const std::optional intersect = leaf.body.findIntersection(entityBox))
                {
                    leaf.collision_buffer.push_back(entity);
                }
            }
        }
    }

    std::set<std::pair<UUID, UUID>> Quadtree::GeneratePairs()
    {
        std::set<std::pair<UUID, UUID>> unique_pairs;
        for (auto& leaf : m_GridNodes)
        {
            for (size_t a = 0; a < leaf.collision_buffer.size(); a++)
            {
                for (size_t b = a + 1; b < leaf.collision_buffer.size(); b++)
                {
                    UUID entityA = leaf.collision_buffer[a];
                    UUID entityB = leaf.collision_buffer[b];
                    if (entityA > entityB)
                        std::swap(entityA, entityB);
                    unique_pairs.insert({ entityA, entityB });
                }
            }
        }
        return unique_pairs;
    }
}
