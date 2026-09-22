#pragma once

#include "Core/EntityManager/EntityManager.h"
#include "ECS/Components/MovementComp.h"
#include "ECS/Components/PhysicsComp.h"
#include "ECS/Components/TransformComp.h"

namespace Spoon
{
    struct FrameMotion
    {
        sf::Vector2f currentPosition = { 0.0f, 0.0f };
        sf::Vector2f startPosition = { 0.0f, 0.0f };
        sf::Vector2f delta = { 0.0f, 0.0f };
        bool transformAlreadyAdvanced = false;
    };

    inline FrameMotion ComputeFrameMotion(EntityManager& manager, UUID entity, float dt)
    {
        FrameMotion motion{};
        motion.currentPosition = manager.GetComponent<TransformComp>(entity, TransformComp::Name).GetPosition();

        auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
        auto physicsIt = physicsArray.m_IdToIndex.find(entity);
        if (physicsIt != physicsArray.m_IdToIndex.end())
        {
            auto& physics = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
            motion.transformAlreadyAdvanced = physics.bodyType != BodyType::Static;
            if (physics.bodyType != BodyType::Static)
                motion.delta = physics.velocity * dt;
        }

        auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
        auto movementIt = movementArray.m_IdToIndex.find(entity);
        if (movementIt != movementArray.m_IdToIndex.end())
        {
            auto& movement = manager.GetComponent<MovementComp>(entity, MovementComp::Name);
            const bool hasPhysics = physicsIt != physicsArray.m_IdToIndex.end();
            if (!hasPhysics || manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name).bodyType != BodyType::Static)
                motion.delta = movement.m_ProposedDelta;
            if (!hasPhysics)
                motion.transformAlreadyAdvanced = physicsArray.m_Components.empty();
        }

        motion.startPosition = motion.transformAlreadyAdvanced ? motion.currentPosition - motion.delta : motion.currentPosition;
        return motion;
    }
}
