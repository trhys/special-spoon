#include "SystemRegistry.h"
#include "System/AnimationSystem.h"
#include "System/MovementSystem.h"
#include "System/Physics/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Physics/SystemConfigs.h"

namespace Spoon
{
    std::unique_ptr<ISystem> LoadAnimationSystem(const json* systemData)
    {
        return std::make_unique<AnimationSystem>();
    }

    std::unique_ptr<ISystem> LoadMovementSystem(const json* systemData)
    {
        return std::make_unique<MovementSystem>();
    }

    std::unique_ptr<ISystem> LoadPhysicsSystem(const json* systemData)
    {
        if (!systemData) { return std::make_unique<PhysicsSystem>(); }
        return std::make_unique<PhysicsSystem>(systemData->get<PhysicsSystemConfig>());
    }

    std::unique_ptr<ISystem> LoadCollisionSystem(const json* systemData)
    {
        if (!systemData) { return std::make_unique<CollisionSystem>(); }
        return std::make_unique<CollisionSystem>(systemData->get<CollisionSystemConfig>());
    }

    void RegisterDefaultSystems()
    {
        SS_DEBUG_LOG("[SYSTEM] Registering default systems...")
        SystemRegistry::Get().RegisterLoader("Animation", &LoadAnimationSystem);
        SystemRegistry::Get().RegisterLoader("Movement", &LoadMovementSystem);
        SystemRegistry::Get().RegisterLoader("Collision", &LoadCollisionSystem);
        SystemRegistry::Get().RegisterLoader("Physics", &LoadPhysicsSystem);
    }
}
