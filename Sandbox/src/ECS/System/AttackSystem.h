#pragma once

#include "Spoon.h"

class AttackSystem : public Spoon::ISystem
{
public:
    AttackSystem() : Spoon::ISystem::ISystem("Attack") {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& attackArray = manager.GetArray<AttackComp>(AttackComp::Name);
        auto& statusArray = manager.GetArray<Spoon::StatusComp>(Spoon:StatusComp::Name);
        for (size_t index = 0; index < attackArray.m_Components.size(); index++)
        {
            AttackComp& attackComp = attackArray.m_Components[index];
            Spoon::UUID id = attackArray.m_IndexToId[index];

            if (!attackComp.isAttacking)
                continue;

            if (statusArray.m_IdToIndex.count(id))
            {
                Spoon::StatusComp& statusComp = *statusArray.GetRawComp(id);
                statusComp.m_CurrentState = "attack";
            }
            
        }
    }
};