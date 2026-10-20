#pragma once

#include "Spoon.h"

class CombatSystem : public Spoon::ISystem
{
public:
    CombatSystem() : Spoon::ISystem::ISystem("Combat") {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        // fetch action queue and arrays
        auto& actions = Spoon::Application::Get().GetActionQueue();
        auto& attackArray = manager.GetArray<AttackComp>(AttackComp::Name);
        auto& statusArray = manager.GetArray<Spoon::StatusComp>(Spoon::StatusComp::Name);

        // get attack action for filtering
        const auto attack = Spoon::ActionRegistry::Get().GetAction("Attack");

        for (size_t index = 0; index < attackArray.m_Components.size(); index++)
        {
            AttackComp& attackComp = attackArray.m_Components[index];
            Spoon::UUID id = attackArray.m_IndexToId[index];

            // check for attack action
            if (!actions.HasAction(attack, id))
                continue;

            // apply attack state if entity has an attack action
            if (statusArray.m_IdToIndex.count(id))
            {
                auto& statusComp = statusArray.m_Components[statusArray.m_IdToIndex[id]];
                statusComp.m_CurrentState = Spoon::StateRegistry::Get().GetState("Attack");
            }
            
        }
    }
};