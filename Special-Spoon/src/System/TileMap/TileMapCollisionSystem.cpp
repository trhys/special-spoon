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
        for (auto& tileMap : tileMaps)
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
        m_CacheValid = true;
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