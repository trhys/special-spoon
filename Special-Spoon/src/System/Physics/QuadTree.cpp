#include "QuadTree.h"
#include "Core/EntityManager/EntityManager.h"
#include "FrameMotion.h"
#include "ECS/Components/TransformComp.h"

#include <algorithm>
#include <optional>

namespace Spoon
{
    namespace
    {
        sf::FloatRect ComputeSweptBounds(const sf::FloatRect& startBounds, const sf::Vector2f& delta)
        {
            const sf::Vector2f minPosition = {
                std::min(startBounds.position.x, startBounds.position.x + delta.x),
                std::min(startBounds.position.y, startBounds.position.y + delta.y)
            };
            const sf::Vector2f maxPosition = {
                std::max(startBounds.position.x + startBounds.size.x, startBounds.position.x + delta.x + startBounds.size.x),
                std::max(startBounds.position.y + startBounds.size.y, startBounds.position.y + delta.y + startBounds.size.y)
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

    void Quadtree::Populate(EntityManager& manager)
    {
        Populate(manager, 0.0f, false, nullptr);
    }

    void Quadtree::PopulateSwept(EntityManager& manager, float dt, const MotionProvider& motionProvider)
    {
        Populate(manager, dt, true, motionProvider);
    }

    void Quadtree::Populate(EntityManager& manager, float dt, bool useSweptBounds, const MotionProvider& motionProvider)
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
                // Prefer the caller-supplied motion provider (e.g. CollisionSystem's per-frame
                // cache) so the broadphase agrees with the narrowphase on each entity's current
                // remaining motion. Without this, a second/later resolution pass could build swept
                // bounds from a stale, unclamped delta while narrowphase already knows the entity
                // was clamped - causing pairs to be missed and bodies to tunnel through under
                // multiple simultaneous collisions.
                const FrameMotion motion = motionProvider
                    ? motionProvider(manager, entity, dt)
                    : ComputeFrameMotion(manager, entity, dt);
                entityBox = ComputeSweptBounds(
                    collider.GetWorldBounds(motion.startPosition),
                    motion.delta);
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
