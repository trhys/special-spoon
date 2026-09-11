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
            for (size_t index = 0; index < physicsArray.m_Components.size(); index++)
            {
                auto& physicsComp = physicsArray.m_Components[index];
                UUID id = physicsArray.m_IndexToId[index];
                if (!transformArray.m_IdToIndex.count(id))
                    continue;

                if (physicsComp.bodyType != BodyType::Dynamic)
                    continue;

                physicsComp.velocity.y += k_Gravity * physicsComp.gravityScale * dt;

                const float dampingFactor = std::max(0.0f, 1.0f - physicsComp.linearDamping * dt);
                physicsComp.velocity *= dampingFactor;

                const sf::Vector2f delta = physicsComp.velocity * dt;
                if (delta.x == 0.0f && delta.y == 0.0f)
                    continue;

                TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                transform.Move(delta);
            }
        }

    private:
        static constexpr float k_Gravity = 980.0f;
    };
}
