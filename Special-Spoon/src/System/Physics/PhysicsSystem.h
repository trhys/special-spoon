#pragma once

#include "System/System.h"
#include "QuadTree.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"

#include <optional>

namespace Spoon
{
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem() : ISystem::ISystem("Physics") {}
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            (void)tick;

            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            for (auto& physicsComp : physicsArray.m_Components)
            {
                physicsComp.CollisionHandled();
            }

            if (physicsArray.m_Components.size() < 2)
                return;

            sf::Vector2u windowSize = Application::Get().GetWindow().getSize();
            quadtree.BuildTree({ static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) });
            quadtree.Populate(manager);

            for (const auto& [entityA, entityB] : quadtree.GeneratePairs())
            {
                PhysicsComp& physA = manager.GetComponent<PhysicsComp>(entityA, PhysicsComp::Name);
                PhysicsComp& physB = manager.GetComponent<PhysicsComp>(entityB, PhysicsComp::Name);

                const std::optional collision = physA.GetCollisionBox().findIntersection(physB.GetCollisionBox());
                if (!collision)
                    continue;

                if (physA.isStatic && physB.isStatic)
                    continue;

                physA.CollisionDetected();
                physB.CollisionDetected();

                sf::Vector2f correction = ComputeCorrection(physA.GetCollisionBox(), physB.GetCollisionBox(), *collision);
                if (correction.x == 0.0f && correction.y == 0.0f)
                    continue;

                if (physA.isStatic)
                {
                    ApplyCorrection(manager, entityB, correction * -1.0f);
                }
                else if (physB.isStatic)
                {
                    ApplyCorrection(manager, entityA, correction);
                }
                else
                {
                    ApplyCorrection(manager, entityA, correction * 0.5f);
                    ApplyCorrection(manager, entityB, correction * -0.5f);
                }
            }
        }

        void OnInit()
        {
            // Get config and build tree
        }

    private:
        static sf::Vector2f ComputeCorrection(const sf::FloatRect& boxA, const sf::FloatRect& boxB, const sf::FloatRect& overlap)
        {
            sf::Vector2f centerA = boxA.position + boxA.size / 2.0f;
            sf::Vector2f centerB = boxB.position + boxB.size / 2.0f;

            if (overlap.size.x <= overlap.size.y)
            {
                return { centerA.x < centerB.x ? -overlap.size.x : overlap.size.x, 0.0f };
            }

            return { 0.0f, centerA.y < centerB.y ? -overlap.size.y : overlap.size.y };
        }

        static void ApplyCorrection(EntityManager& manager, UUID entity, const sf::Vector2f& correction)
        {
            if (correction.x == 0.0f && correction.y == 0.0f)
                return;

            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);

            if (transformArray.m_IdToIndex.count(entity))
            {
                TransformComp& transform = manager.GetComponent<TransformComp>(entity, TransformComp::Name);
                transform.Move(correction);
            }

            if (physicsArray.m_IdToIndex.count(entity))
            {
                PhysicsComp& physics = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
                physics.m_CollisionBox.position += correction;
                physics.m_LastCorrection += correction;
                physics.m_BlockedX = physics.m_BlockedX || correction.x != 0.0f;
                physics.m_BlockedY = physics.m_BlockedY || correction.y != 0.0f;
            }

            if (movementArray.m_IdToIndex.count(entity))
            {
                MovementComp& movement = manager.GetComponent<MovementComp>(entity, MovementComp::Name);
                movement.m_WasCorrectedByPhysics = true;
            }
        }

        Quadtree quadtree;
    };
}