#pragma once

#include "System/System.h"

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
        void BuildColliderCache(EntityManager& manager);
        void MergeColliderCache();

        // generate entities in the ECS for each merged collider
        void GenerateColliderEntities(EntityManager& manager);
        void KillColliderEntities(EntityManager& manager);

    private:
        std::vector<TileCollider> m_TileColliders;
        std::vector<UUID> m_CachedEntities;
    };
}