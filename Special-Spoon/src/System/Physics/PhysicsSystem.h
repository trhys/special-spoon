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
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem() : ISystem::ISystem("Physics") {}
        ~PhysicsSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            (void)tick;

            // Here we need to resolve state flags to zero before processing
            // and calibrate the collision box and transform coords 
            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            for (size_t index = 0; index < physicsArray.m_Components.size(); index++)
            {
                auto& physicsComp = physicsArray.m_Components[index];
                UUID id = physicsArray.m_IndexToId[index];
                physicsComp.CollisionHandled();

                TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                physicsComp.SetPosition(transform.GetPosition());
            }

            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            for (auto& movementComp : movementArray.m_Components)
            {
                movementComp.m_WasCorrectedByPhysics = false;
            }

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

                    appliedCorrection = true;
                }

                if (!appliedCorrection)
                    break;
            }
        }

        void OnInit()
        {
            // Get config and build tree
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

        Quadtree quadtree;
    };
}
