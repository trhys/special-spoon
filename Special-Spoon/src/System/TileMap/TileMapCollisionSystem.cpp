#include "TileMapCollisionSystem.h"
#include "ECS/ECS.h"

namespace Spoon
{
    void TileMapCollisionSystem::Update(sf::Time tick, EntityManager& manager)
    {
        auto& tileMaps = manager.GetArray<TileMapComp>(TileMapComp::Name);
        for (auto& tileMap : tileMaps.m_Components)
        {
            if (tileMap.m_RebuildCollision)
            {
                KillColliderEntities(manager, tileMap.m_ColliderEntities);
                std::vector<TileCollider> colliders = BuildColliderCache(tileMap);
                GenerateColliderEntities(manager, tileMap, colliders);
                tileMap.m_RebuildCollision = false;
            }
        }
    }

    std::vector<TileCollider> TileMapCollisionSystem::BuildColliderCache(TileMapComp& tileMap)
    {
        std::vector<TileCollider> tileColliders;
        // extracts horizontal runs of collidable tiles
        // into merged rects to create colliders from
        const int mapWidth = tileMap.m_MapSize.x;
        const int mapHeight = tileMap.m_MapSize.y;
        const int tileWidth = tileMap.m_Atlas.tileWidth;
        const int tileHeight = tileMap.m_Atlas.tileHeight;

        if (mapWidth <= 0 ||
            mapHeight <= 0 ||
            tileWidth <= 0 ||
            tileHeight <= 0)
        {
            return tileColliders;
        }

        const std::size_t expectedTileCount =
            static_cast<std::size_t>(mapWidth) *
            static_cast<std::size_t>(mapHeight);

        for (std::size_t layerIndex = 0;
            layerIndex < tileMap.m_Layers.size();
            ++layerIndex)
        {
            const TileLayer& layer = tileMap.m_Layers[layerIndex];

            if (!layer.collidable)
                continue;

            for (int y = 0; y < mapHeight; ++y)
            {
                int runStartX = -1;

                for (int x = 0; x <= mapWidth; ++x)
                {
                    bool solid = false;

                    // x == mapWidth is a sentinel column. It forces any
                    // active run to be emitted at the end of the row.
                    if (x < mapWidth)
                    {
                        const std::size_t tileIndex =
                            static_cast<std::size_t>(y) *
                                static_cast<std::size_t>(mapWidth) +
                            static_cast<std::size_t>(x);

                        if (tileIndex < layer.tiles.size() &&
                            tileIndex < expectedTileCount)
                        {
                            const Tile& tile = layer.tiles[tileIndex];
                            solid = tile.id != 0;
                        }
                    }

                    if (solid)
                    {
                        // Begin a new horizontal run.
                        if (runStartX < 0)
                            runStartX = x;

                        continue;
                    }

                    // Current cell is empty, or this is the sentinel column.
                    // Close the active run if one exists.
                    if (runStartX >= 0)
                    {
                        const int runWidth = x - runStartX;

                        TileCollider collider;
                        collider.body = sf::FloatRect{
                            sf::Vector2f{
                                static_cast<float>(
                                    runStartX * tileWidth
                                ),
                                static_cast<float>(
                                    y * tileHeight
                                )
                            },
                            sf::Vector2f{
                                static_cast<float>(
                                    runWidth * tileWidth
                                ),
                                static_cast<float>(tileHeight)
                            }
                        };

                        tileColliders.push_back(collider);
                        runStartX = -1;
                    }
                }
            }
        }
        return tileColliders;
    }

    void TileMapCollisionSystem::GenerateColliderEntities(EntityManager& manager, TileMapComp& tileMap, std::vector<TileCollider>& colliders)
    {
        for (auto& collider : colliders)
        {
            auto id = manager.CreateEntity();
            tileMap.m_ColliderEntities.push_back(id);
            manager.MakeComponent<ColliderComp>(id, ColliderComp::Name, collider.body.size);
            manager.MakeComponent<TransformComp>(id, TransformComp::Name, collider.body.position);
            manager.MakeComponent<PhysicsComp>(id, PhysicsComp::Name, BodyType::Static);
        }
    }

    void TileMapCollisionSystem::KillColliderEntities(EntityManager& manager, std::vector<UUID>& cachedEntities)
    {
        for (UUID id : cachedEntities)
        {
            manager.KillEntity(id);
        }
        cachedEntities.clear();
    }

    void TileMapCollisionSystem::OnReflect()
    {
        // Implementation of the reflection logic for the system
    }
}