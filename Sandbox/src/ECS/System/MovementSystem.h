#pragma once

#include "Spoon.h"

class MovementSystem : public Spoon::ISystem
{
public:
    MovementSystem() : Spoon::ISystem::ISystem("Movement") {}
    ~MovementSystem() {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
        auto& transformArray = manager.GetArray<Spoon::TransformComp>(Spoon::TransformComp::Name);

        for(size_t index = 0; index < movementArray.m_Components.size(); index++)
        {
            MovementComp& moveComp = movementArray.m_Components[index];
            Spoon::UUID ID = movementArray.m_IndexToId[index];

            if(transformArray.m_IdToIndex.count(ID))
            {
                Spoon::TransformComp& transComp = manager.GetComponent<Spoon::TransformComp>(ID);
                float targetX = transComp.GetPosition().x + moveComp.m_Velocity.x * tick.asSeconds();
                float targetY = transComp.GetPosition().y + moveComp.m_Velocity.y * tick.asSeconds();
                transComp.SetPosition(sf::Vector2f(targetX, targetY));

                // Determine direction of travel
                if(moveComp.m_Velocity.x > 0) { moveComp.m_FacingLR = "Right"; }
                else { moveComp.m_FacingLR = "Left"; }
                if(moveComp.m_Velocity.y > 0) { moveComp.m_FacingUD = "Up"; }
                else { moveComp.m_FacingUD = "Down"; }
            }
            else
            {
                SS_DEBUG_LOG("[MOVEMENT SYSTEM] Entity has no transform!")
            }
            
        }
    }
};