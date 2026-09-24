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

        std::vector<std::string> RunBefore() override
        {
            return { "Collision" };
        }

        void Update(sf::Time tick, EntityManager& manager) override;
        void OnReflect() override;

        void BuildColliderCache(EntityManager& manager);
        void ClearColliderCache();

    private:
        std::vector<TileCollider> m_TileColliders;

        bool m_CacheValid = false;
    };
}