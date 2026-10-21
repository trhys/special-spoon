#pragma once 

#include "Spoon.h"
#include "../Component/Combat/RespawnComp.h"

class RespawnSystem : public Spoon::ISystem<RespawnSystem>
{
public:
    RespawnSystem() : Spoon::ISystem<RespawnSystem>(Name) {}
    
    static constexpr const char* Name = "Respawn";

    void Update(sf::Time tick, Spoon::EntityManager& entityManager) override
    {
        auto& respawnArray = entityManager.GetComponentArray<RespawnComp>();

        for (auto& [id, index] : respawnArray.m_IdToIndex)
        {
            RespawnComp& respawn = respawnArray.m_Components[index];

            if (respawn.isRespawning)
            {
                respawn.respawnTimer += tick.asSeconds();
                if (respawn.respawnTimer >= respawn.respawnTime)
                {
                    respawn.isRespawning = false;
                    respawn.respawnTimer = 0.0f;
                    // todo: respawn the entity at respawn.respawnPosition
                }
            }
        }
    }
};