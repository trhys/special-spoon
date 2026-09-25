#pragma once

#include "System/System.h"
#include "ECS/Components/World/TileMapComp.h"

namespace Spoon
{
    struct TileCollider
    {
        sf::FloatRect body;
    };

    class TileMapCollisionSystem : public ISystem
    {
    public:
        TileMapCollisionSystem() :
            ISystem("TileMapCollision") {}

        std::vector<std::string> RunBefore() const override
        {
            return { "Collision" };
        }

        void Update(sf::Time tick, EntityManager& manager) override;
        void OnReflect() override;

        // create the rects representing the total
        // space occupied by collidable tiles in the tile map
        std::vector<TileCollider> BuildColliderCache(TileMapComp& tileMap);

        // generate/kill entities in the ECS for each merged collider
        void GenerateColliderEntities(EntityManager& manager, TileMapComp& tileMap, std::vector<TileCollider>& colliders);
        void KillColliderEntities(EntityManager& manager, std::vector<UUID>& cachedEntities);
    };
}