#include "TileMapCollisionSystem.h"
#include "ECS/ECS.h"

namespace Spoon
{
    void TileMapCollisionSystem::Update(sf::Time tick, EntityManager& manager)
    {
        if (!m_CacheValid)
            BuildColliderCache(manager);
    }

    void TileMapCollisionSystem::BuildColliderCache(EntityManager& manager)
    {
        if (m_CacheValid)
            return;

        m_TileColliders.clear();
        auto& tileMaps = manager.GetArray<TileMapComp>(TileMapComp::Name);
        for (auto& tileMap : tileMaps.m_Components)
        {
            auto& layers = tileMap.m_Layers;
            for (size_t layerIndex = 0; layerIndex < layers.size(); ++layerIndex)
            {
                auto& layer = layers[layerIndex];
                if (!layer.collidable)
                    continue;

                auto& tiles = layer.tiles;
                for (size_t index = 0; index < tiles.size(); ++index)
                {
                    if (tiles[index].collidable)
                    {
                        TileCollider collider;
                        int tileX = index % tileMap.m_MapSize.x;
                        int tileY = index / tileMap.m_MapSize.x;
                        collider.body = tileMap.GetTileBounds(tileX, tileY, static_cast<int>(layerIndex));
                        m_TileColliders.push_back(collider);
                    }
                }
            }
        }
        MergeColliderCache();
        m_CacheValid = true;
    }

    void TileMapCollisionSystem::MergeColliderCache()
    {
        std::vector<TileCollider> mergedColliders;
        for (size_t index = 0; index < m_TileColliders.size(); ++index)
        {
            auto& collider = m_TileColliders[index];
            if (index + 1 >= m_TileColliders.size())
                break;

            auto& next = m_TileColliders[index + 1];
            float rightHandSide = collider.body.position.x + collider.body.size.x;
            float nextLeftHandSide = next.body.position.x;

            if (rightHandSide == nextLeftHandSide)
            {
                collider.body.size.x += next.body.size.x;
                ++index;
            }
            mergedColliders.push_back(collider);

        }
        m_TileColliders = std::move(mergedColliders);
    }

    void TileMapCollisionSystem::GenerateColliderEntities(EntityManager& manager)
    {
        for (auto& collider : m_TileColliders)
        {
            auto id = manager.CreateEntity();
            m_CachedEntities.push_back(id);
            manager.MakeComponent<ColliderComp>(id, ColliderComp::Name, collider.body.size);
            manager.MakeComponent<TransformComp>(id, TransformComp::Name, collider.body.position);
            manager.MakeComponent<PhysicsComp>(id, PhysicsComp::Name, BodyType::Static);
        }
    }

    void TileMapCollisionSystem::KillColliderEntities(EntityManager& manager)
    {
        for (UUID id : m_CachedEntities)
        {
            manager.KillEntity(id);
        }
        m_CachedEntities.clear();
    }

    void TileMapCollisionSystem::ClearColliderCache()
    {
        m_TileColliders.clear();
        m_CacheValid = false;
    }

    void TileMapCollisionSystem::OnReflect()
    {
        // Implementation of the reflection logic for the system
    }
}