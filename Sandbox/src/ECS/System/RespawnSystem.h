#pragma once 

#include "Spoon.h"
#include "../Component/Combat/RespawnComp.h"
#include "../Component/Combat/AttackComp.h"
#include "../Component/Combat/HealthComp.h"

class RespawnSystem : public Spoon::ISystem
{
public:
    RespawnSystem() : Spoon::ISystem(Name) {}
    
    static constexpr const char* Name = "Respawn";

    void Update(sf::Time tick, Spoon::EntityManager& entityManager) override
    {
        auto& actions = Spoon::Application::Get().GetActionQueue();

        auto& respawnArray = entityManager.GetArray<RespawnComp>(RespawnComp::Name);

        for (auto& [id, index] : respawnArray.m_IdToIndex)
        {
            RespawnComp& respawn = respawnArray.m_Components[index];

            if (actions.HasAction(Spoon::ActionRegistry::Get().GetAction("Die"), id))
            {
                respawn.isRespawning = true;
                respawn.respawnTimer = respawn.respawnTime;
                StopEntity(entityManager, id);
                continue; // dont decrement the timer same frame as death
            }

            if (respawn.isRespawning)
            {
                respawn.respawnTimer -= tick.asSeconds();
                if (respawn.respawnTimer <= 0.0f)
                    ResetDeadEntity(entityManager, id);
            }
        }
    }

    // helper for stopping persistant states upstream when
    // and entity is dead
    void StopEntity(Spoon::EntityManager& entityManager, Spoon::UUID id)
    {
        auto& attackArray = entityManager.GetArray<AttackComp>(AttackComp::Name);
        auto& movementArray = entityManager.GetArray<Spoon::MovementComp>(Spoon::MovementComp::Name);
        if (attackArray.m_IdToIndex.count(id))
        {
            // cancel active attack cycle, but let
            // active hitboxes expire naturally
            AttackComp& attack = attackArray.m_Components[attackArray.m_IdToIndex[id]];
            attack.isAttacking = false;
            attack.cooldownTimer = 0.0f;
            attack.phaseTimer = 0.0f;
        }
        if (movementArray.m_IdToIndex.count(id))
        {
            Spoon::MovementComp& movement = movementArray.m_Components[movementArray.m_IdToIndex[id]];
            movement.m_Velocity = sf::Vector2f(0.0f, 0.0f);
        }
    }

    // revive entity when respawning
    void ResetDeadEntity(Spoon::EntityManager& entityManager, Spoon::UUID id)
    {
        auto& respawnArray = entityManager.GetArray<RespawnComp>(RespawnComp::Name);
        auto& healthArray = entityManager.GetArray<HealthComp>(HealthComp::Name);
        auto& transformArray = entityManager.GetArray<Spoon::TransformComp>(Spoon::TransformComp::Name);
        auto& statusArray = entityManager.GetArray<Spoon::StatusComp>(Spoon::StatusComp::Name);

        if (!respawnArray.m_IdToIndex.count(id))
            return;
        
        RespawnComp& respawn = respawnArray.m_Components[respawnArray.m_IdToIndex[id]];
        respawn.isRespawning = false;
        respawn.respawnTimer = 0.0f;


        if (healthArray.m_IdToIndex.count(id))
        {
            HealthComp& health = healthArray.m_Components[healthArray.m_IdToIndex[id]];
            health.m_CurrentHealth = health.m_MaxHealth;
            health.isAlive = true;
            // set temp invincibility frames on respawn
            health.iFrameTimer = 3.0f;
            health.isInvincible = true;
            // clear requests
            health.damageQueue = {};
            health.healQueue = {};
        }

        if (transformArray.m_IdToIndex.count(id))
        {
            Spoon::TransformComp& transform = transformArray.m_Components[transformArray.m_IdToIndex[id]];
            transform.SetPosition(respawn.respawnPosition);
        }

        if (statusArray.m_IdToIndex.count(id))
        {
            Spoon::StatusComp& status = statusArray.m_Components[statusArray.m_IdToIndex[id]];
            status.ApplyState(Spoon::StateRegistry::Get().GetState("Respawning"));
        }
    }
};