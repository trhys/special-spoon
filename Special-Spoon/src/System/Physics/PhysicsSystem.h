#pragma once

#include "System/System.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"

#include <algorithm>

namespace Spoon
{
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem() : ISystem::ISystem("Physics") {}
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            const float dt = tick.asSeconds();
            if (dt <= 0.0f)
                return;

            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            for (size_t index = 0; index < physicsArray.m_Components.size(); index++)
            {
                auto& physicsComp = physicsArray.m_Components[index];
                UUID id = physicsArray.m_IndexToId[index];
                if (!transformArray.m_IdToIndex.count(id))
                    continue;

                if (physicsComp.bodyType == BodyType::Static)
                    continue;

                if (movementArray.m_IdToIndex.count(id))
                {
                    auto& movementComp = manager.GetComponent<MovementComp>(id, MovementComp::Name);
                    physicsComp.velocity.x = movementComp.m_Velocity.x;
                    if (physicsComp.bodyType == BodyType::Kinematic || movementComp.m_FrameIntent.y != 0.0f || physicsComp.gravityScale == 0.0f)
                    {
                        physicsComp.velocity.y = movementComp.m_Velocity.y;
                    }
                }

                if (physicsComp.bodyType == BodyType::Dynamic)
                {
                    physicsComp.velocity.y += k_Gravity * physicsComp.gravityScale * dt;

                    const float dampingFactor = std::max(0.0f, 1.0f - physicsComp.linearDamping * dt);
                    physicsComp.velocity *= dampingFactor;
                }

                const sf::Vector2f delta = physicsComp.velocity * dt;
                if (delta.x == 0.0f && delta.y == 0.0f)
                {
                    if (movementArray.m_IdToIndex.count(id))
                    {
                        auto& movementComp = manager.GetComponent<MovementComp>(id, MovementComp::Name);
                        movementComp.m_ProposedDelta = delta;
                    }
                    continue;
                }

                TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                transform.Move(delta);

                if (movementArray.m_IdToIndex.count(id))
                {
                    auto& movementComp = manager.GetComponent<MovementComp>(id, MovementComp::Name);
                    movementComp.m_ProposedDelta = delta;
                }
            }
        }

    private:
        static constexpr float k_Gravity = 980.0f;
    };
}
