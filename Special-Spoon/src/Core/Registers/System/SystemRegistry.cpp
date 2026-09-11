#include "SystemRegistry.h"
#include "System/AnimationSystem.h"
#include "System/Physics/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"

namespace Spoon
{
    std::unique_ptr<ISystem> LoadAnimationSystem(const json* systemData)
    {
        return std::make_unique<AnimationSystem>();
    }

    std::unique_ptr<ISystem> LoadPhysicsSystem(const json* systemData)
    {
        return std::make_unique<PhysicsSystem>();
    }

    std::unique_ptr<ISystem> LoadCollisionSystem(const json* systemData)
    {
        return std::make_unique<CollisionSystem>();
    }

    void RegisterDefaultSystems()
    {
        SS_DEBUG_LOG("[SYSTEM] Registering default systems...")
        SystemRegistry::Get().RegisterLoader("Animation", &LoadAnimationSystem);
        SystemRegistry::Get().RegisterLoader("Physics", &LoadPhysicsSystem);
        SystemRegistry::Get().RegisterLoader("Collision", &LoadCollisionSystem);
    }
}
