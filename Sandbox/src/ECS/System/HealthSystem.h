#pragma once

#include "Spoon.h"

#include "ECS/Component/Combat/HealthComp.h"

class HealthSystem : public Spoon::ISystem
{
public:
    HealthSystem() : Spoon::ISystem::ISystem("Health") {}

    std::vector<std::string> RunBefore() const override {
        return { "Animation" };
    }

    std::vector<std::string> RunAfter() const override {
        return { "Combat"};
    }

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& healthArray = manager.GetArray<HealthComp>(HealthComp::Name);
        auto& statusArray = manager.GetArray<Spoon::StatusComp>(Spoon::StatusComp::Name);

        for (size_t index = 0; index < healthArray.m_Components.size(); index++)
        {
            HealthComp& health = healthArray.m_Components[index];
            Spoon::UUID id = healthArray.m_IndexToId[index];
            
            // process iframes
            if (health.iFrameTimer > 0.0f) {
                health.iFrameTimer -= tick.asSeconds();
                if (health.iFrameTimer < 0.0f)
                    health.iFrameTimer = 0.0f;
            }

            // gate invuln
            if (health.iFrameTimer == 0.0f)
                health.isInvincible = false;

            // process heal requests
            while (!health.healQueue.empty())
            {
                HealRequest request = health.healQueue.front();
                health.healQueue.pop();
                health.m_CurrentHealth += request.healAmount;
                if (health.m_CurrentHealth > health.m_MaxHealth)
                    health.m_CurrentHealth = health.m_MaxHealth;
            }

            // process damage requests and apply iframes
            while (!health.damageQueue.empty())
            {
                DamageRequest request = health.damageQueue.front();
                health.damageQueue.pop();
                if (!health.isInvincible)
                {
                    health.m_CurrentHealth -= request.damageAmount;
                    health.isInvincible = true;
                    health.iFrameTimer = health.iFramesDuration;
                }
            }

            // life check
            if (health.m_CurrentHealth <= 0)
                health.isAlive = false;

            if (statusArray.m_IdToIndex.find(id) != statusArray.m_IdToIndex.end() && !health.isAlive)
            {
                Spoon::StatusComp& status = statusArray.m_Components[statusArray.m_IdToIndex[id]];
                status.m_CurrentState = Spoon::StateRegistry::Get().GetState("Dead");
            }
        }
    }

    void OnReflect() override
    {
    }
};