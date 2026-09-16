#include "SystemRegistry.h"
#include "System/AnimationSystem.h"
#include "System/Physics/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Physics/PhysicsSystemConfig.h"

namespace Spoon
{
    std::unique_ptr<ISystem> LoadAnimationSystem(const json* systemData)
    {
        return std::make_unique<AnimationSystem>();
    }

    std::unique_ptr<ISystem> LoadPhysicsSystem(const json* systemData)
    {
        if (!systemData) { return std::make_unique<PhysicsSystem>(); }

        return std::make_unique<PhysicsSystem>(PhysicsSystemConfig{
            systemData->at("linear damping").get<float>(),
            systemData->at("friction").get<float>(),
            systemData->at("restitution").get<float>(),
            systemData->at("max speed").get<float>(),
            systemData->at("sleep threshold").get<float>(),
            systemData->at("enable sleep snap").get<bool>(),
            systemData->at("clamp neg inputs").get<bool>()
            });
    }

    std::unique_ptr<ISystem> LoadCollisionSystem(const json* systemData)
    {
        return std::make_unique<CollisionSystem>();
    }

    void RegisterDefaultSystems()
    {
        SS_DEBUG_LOG("[SYSTEM] Registering default systems...")
        SystemRegistry::Get().RegisterLoader("Animation", &LoadAnimationSystem);
        SystemRegistry::Get().RegisterLoader("Collision", &LoadCollisionSystem);
        SystemRegistry::Get().RegisterLoader("Physics", &LoadPhysicsSystem);
    }
}
