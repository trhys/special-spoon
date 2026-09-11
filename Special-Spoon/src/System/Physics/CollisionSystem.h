#pragma once

#include "System/System.h"
#include "QuadTree.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"

#include <optional>
#include <array>
#include <cmath>

namespace Spoon
{
    class CollisionSystem : public ISystem
    {
    public:
        CollisionSystem() : ISystem::ISystem("Collision") {}
        ~CollisionSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            (void)tick;

            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            if (physicsArray.m_Components.size() < 2)
                return;

            constexpr int maxIterations = 8;
            for (int iteration = 0; iteration < maxIterations; iteration++)
            {
                bool appliedCorrection = false;

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

                    physA.CollisionDetected();
                    physB.CollisionDetected();

                    if (physA.isStatic && physB.isStatic)
                        continue;

                    sf::Vector2f correction = ComputeCorrection(physA.GetCollisionBox(), physB.GetCollisionBox());
                    if (correction.x == 0.0f && correction.y == 0.0f)
                        continue;

                    const float restitution = (physA.restitution + physB.restitution) * 0.5f;
                    if (physA.isStatic)
                    {
                        const sf::Vector2f correctionB = correction * -1.0f;
                        ApplyCorrection(manager, entityB, correctionB);
                        ResolveVelocity(manager, entityB, correctionB, restitution);
                    }
                    else if (physB.isStatic)
                    {
                        ApplyCorrection(manager, entityA, correction);
                        ResolveVelocity(manager, entityA, correction, restitution);
                    }
                    else
                    {
                        const sf::Vector2f correctionA = correction * 0.5f;
                        const sf::Vector2f correctionB = correction * -0.5f;
                        ApplyCorrection(manager, entityA, correctionA);
                        ApplyCorrection(manager, entityB, correctionB);
                        ResolveVelocity(manager, entityA, correctionA, restitution);
                        ResolveVelocity(manager, entityB, correctionB, restitution);
                    }

                    appliedCorrection = true;
                }

                if (!appliedCorrection)
                    break;
            }
        }

    private:
        static sf::Vector2f ComputeCorrection(const sf::FloatRect& boxA, const sf::FloatRect& boxB)
        {
            const float moveLeft = boxB.position.x - (boxA.position.x + boxA.size.x);
            const float moveRight = (boxB.position.x + boxB.size.x) - boxA.position.x;
            const float moveUp = boxB.position.y - (boxA.position.y + boxA.size.y);
            const float moveDown = (boxB.position.y + boxB.size.y) - boxA.position.y;

            const std::array xMoves{ moveLeft, moveRight };
            const std::array yMoves{ moveUp, moveDown };

            float bestX = std::abs(xMoves[0]) <= std::abs(xMoves[1]) ? xMoves[0] : xMoves[1];
            float bestY = std::abs(yMoves[0]) <= std::abs(yMoves[1]) ? yMoves[0] : yMoves[1];

            if (std::abs(bestX) <= std::abs(bestY))
                return { bestX, 0.0f };

            return { 0.0f, bestY };
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

        static void ResolveVelocity(EntityManager& manager, UUID entity, const sf::Vector2f& correction, float restitution)
        {
            if (correction.x == 0.0f && correction.y == 0.0f)
                return;

            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            if (!physicsArray.m_IdToIndex.count(entity))
                return;

            PhysicsComp& physics = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
            if (physics.isStatic)
                return;

            const float length = std::sqrt(correction.x * correction.x + correction.y * correction.y);
            if (length <= 0.0001f)
                return;

            const sf::Vector2f normal = correction / length;
            const float velocityIntoSurface = physics.m_Velocity.x * normal.x + physics.m_Velocity.y * normal.y;
            if (velocityIntoSurface < 0.0f)
            {
                physics.m_Velocity -= normal * ((1.0f + restitution) * velocityIntoSurface);
            }
        }

        Quadtree quadtree;
    };
}
