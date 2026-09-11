#pragma once

#include "System/System.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"

namespace Spoon
{
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem() : ISystem::ISystem("Physics") {}
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            for (size_t index = 0; index < physicsArray.m_Components.size(); index++)
            {
                auto& physicsComp = physicsArray.m_Components[index];
                UUID id = physicsArray.m_IndexToId[index];
                physicsComp.CollisionHandled();

                if (!transformArray.m_IdToIndex.count(id))
                    continue;

                TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                physicsComp.SetPosition(transform.GetPosition());
            }

            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            for (auto& movementComp : movementArray.m_Components)
            {
                movementComp.m_WasCorrectedByPhysics = false;
            }

            const float deltaSeconds = tick.asSeconds();
            if (deltaSeconds <= 0.0f)
                return;

            for (size_t index = 0; index < physicsArray.m_Components.size(); index++)
            {
                PhysicsComp& physicsComp = physicsArray.m_Components[index];
                UUID id = physicsArray.m_IndexToId[index];

                if (physicsComp.isStatic)
                {
                    physicsComp.m_Velocity = { 0.0f, 0.0f };
                    continue;
                }

                if (!transformArray.m_IdToIndex.count(id))
                    continue;

                TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                const sf::Vector2f delta = physicsComp.m_Velocity * deltaSeconds;
                transform.Move(delta);
                physicsComp.m_CollisionBox.position += delta;

                if (movementArray.m_IdToIndex.count(id))
                {
                    MovementComp& movementComp = manager.GetComponent<MovementComp>(id, MovementComp::Name);
                    movementComp.m_ProposedDelta = delta;
                    movementComp.m_Velocity = physicsComp.m_Velocity;
                }
            }
        }
    };
}
