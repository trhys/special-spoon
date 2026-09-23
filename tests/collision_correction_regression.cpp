#include "Core/EntityManager/EntityManager.h"
#include "ECS/Components/ColliderComp.h"
#include "ECS/Components/PhysicsComp.h"
#include "ECS/Components/TransformComp.h"
#include "System/Physics/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"

#include <cmath>
#include <iostream>

namespace
{
    using namespace Spoon;

    constexpr float kEpsilon = 0.001f;

    bool Overlaps(const sf::FloatRect& a, const sf::FloatRect& b)
    {
        return a.position.x < b.position.x + b.size.x &&
               a.position.x + a.size.x > b.position.x &&
               a.position.y < b.position.y + b.size.y &&
               a.position.y + a.size.y > b.position.y;
    }

    UUID CreateBody(EntityManager& manager, const char* name, const sf::Vector2f& position, const sf::Vector2f& size, BodyType bodyType, const sf::Vector2f& velocity = { 0.0f, 0.0f })
    {
        const UUID entity = manager.CreateEntity(name);
        manager.MakeComponent<TransformComp>(entity, TransformComp::Name, position);
        manager.MakeComponent<ColliderComp>(entity, ColliderComp::Name);
        manager.MakeComponent<PhysicsComp>(entity, PhysicsComp::Name);

        auto& collider = manager.GetComponent<ColliderComp>(entity, ColliderComp::Name);
        collider.SetAABBSize(size);

        auto& physics = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
        physics.bodyType = bodyType;
        physics.velocity = velocity;
        physics.mass = 1.0f;
        physics.gravityScale = 0.0f;
        physics.restitution = 0.0f;
        physics.friction = 0.0f;
        physics.linearDamping = 0.0f;
        return entity;
    }
}

int main()
{
    using namespace Spoon;

    EntityManager manager;
    const UUID mover = CreateBody(manager, "mover", { 10.0f, 10.0f }, { 30.0f, 10.0f }, BodyType::Dynamic, { 200.0f, 0.0f });
    const UUID pushed = CreateBody(manager, "pushed", { 46.0f, 10.0f }, { 4.0f, 10.0f }, BodyType::Dynamic);
    const UUID wall = CreateBody(manager, "wall", { 50.0f, 0.0f }, { 1.0f, 40.0f }, BodyType::Static);

    (void)mover;
    (void)wall;

    PhysicsSystemConfig physicsConfig;
    physicsConfig.gravityEnabled = false;
    physicsConfig.enableSleepSnap = false;
    physicsConfig.defaultRestitution = 0.0f;
    physicsConfig.defaultFriction = 0.0f;
    PhysicsSystem physicsSystem(physicsConfig);
    CollisionSystem collisionSystem(CollisionSystemConfig{ sf::Vector2f{ 200.0f, 200.0f } });

    const sf::Time tick = sf::seconds(0.05f);
    const float pushedStartX = manager.GetComponent<TransformComp>(pushed, TransformComp::Name).GetPosition().x;

    physicsSystem.Update(tick, manager);

    const auto& pushedAfterPhysics = manager.GetComponent<TransformComp>(pushed, TransformComp::Name);
    const auto& moverAfterPhysics = manager.GetComponent<TransformComp>(mover, TransformComp::Name);
    if (std::abs(pushedAfterPhysics.GetPosition().x - pushedStartX) > kEpsilon)
    {
        std::cerr << "Expected pushed body to remain stationary before collision resolution, but moved from "
                  << pushedStartX << " to " << pushedAfterPhysics.GetPosition().x << ".\n";
        return 1;
    }

    const sf::FloatRect moverBoundsAfterPhysics = manager.GetComponent<ColliderComp>(mover, ColliderComp::Name).GetWorldBounds(moverAfterPhysics.GetPosition());
    const sf::FloatRect pushedBoundsAfterPhysics = manager.GetComponent<ColliderComp>(pushed, ColliderComp::Name).GetWorldBounds(pushedAfterPhysics.GetPosition());
    if (!Overlaps(moverBoundsAfterPhysics, pushedBoundsAfterPhysics))
    {
        std::cerr << "Expected mover to overlap the pushed body after physics integration so collision correction is exercised.\n";
        return 1;
    }

    collisionSystem.Update(tick, manager);

    const auto& pushedTransform = manager.GetComponent<TransformComp>(pushed, TransformComp::Name);
    const auto& wallTransform = manager.GetComponent<TransformComp>(wall, TransformComp::Name);
    const auto& pushedCollider = manager.GetComponent<ColliderComp>(pushed, ColliderComp::Name);
    const auto& wallCollider = manager.GetComponent<ColliderComp>(wall, ColliderComp::Name);

    const sf::FloatRect pushedBounds = pushedCollider.GetWorldBounds(pushedTransform.GetPosition());
    const sf::FloatRect wallBounds = wallCollider.GetWorldBounds(wallTransform.GetPosition());

    if (Overlaps(pushedBounds, wallBounds))
    {
        std::cerr << "Pushed body still overlaps the wall after collision resolution.\n";
        return 1;
    }

    const float pushedRight = pushedBounds.position.x + pushedBounds.size.x;
    if (pushedRight > wallBounds.position.x + kEpsilon)
    {
        std::cerr << "Pushed body tunneled through the wall. Right edge " << pushedRight
                  << " is beyond wall left edge " << wallBounds.position.x << ".\n";
        return 1;
    }

    return 0;
}
