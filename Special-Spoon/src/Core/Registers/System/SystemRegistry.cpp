#include "SystemRegistry.h"
#include "System/Include.h"

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

    std::unique_ptr<ISystem> LoadTileMapCollisionSystem(const json* systemData)
    {
        return std::make_unique<TileMapCollisionSystem>();
    }

    std::unique_ptr<ISystem> LoadAudioSystem(const json* systemData)
    {
        return std::make_unique<AudioSystem>();
    }

    std::unique_ptr<ISystem> LoadParticleSystem(const json* systemData)
    {
        return std::make_unique<ParticleSystem>();
    }

    void RegisterDefaultSystems()
    {
        SS_DEBUG_LOG("[SYSTEM] Registering default systems...")
        SystemRegistry::Get().RegisterLoader("Animation", &LoadAnimationSystem);
        SystemRegistry::Get().RegisterLoader("Movement", &LoadMovementSystem);
        SystemRegistry::Get().RegisterLoader("Collision", &LoadCollisionSystem);
        SystemRegistry::Get().RegisterLoader("Physics", &LoadPhysicsSystem);
        SystemRegistry::Get().RegisterLoader("TileMapCollision", &LoadTileMapCollisionSystem);
        SystemRegistry::Get().RegisterLoader("Audio", &LoadAudioSystem);
        SystemRegistry::Get().RegisterLoader("Particles", &LoadParticleSystem);
    }
}
