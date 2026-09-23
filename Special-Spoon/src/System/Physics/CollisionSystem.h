#pragma once

#include "ECS/Components/ColliderComp.h"
#include "SFML/Graphics.hpp"

#include "System/System.h"
#include "PhysicsSystem.h"
#include "QuadTree.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Spoon
{
    class CollisionSystem : public ISystem
    {
    public:
        CollisionSystem() : ISystem::ISystem("Collision") {}
        CollisionSystem(const CollisionSystemConfig& config) : ISystem::ISystem("Collision") { m_Config = config; }
        ~CollisionSystem() {}

        json Serialize() override
        {
            json j;
            nlohmann::to_json(j, m_Config);
            return j;
        }

        void Update(sf::Time tick, EntityManager& manager) override
        {
            const float dt = tick.asSeconds();
            if (dt <= 0.0f)
                return;

            auto& colliderArray = manager.GetArray<ColliderComp>(ColliderComp::Name);
            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);

            for (auto& movementComp : movementArray.m_Components)
            {
                movementComp.m_WasCorrectedByPhysics = false;
            }

            for (auto& collider : colliderArray.m_Components)
            {
                collider.touchingLastFrame = collider.touchingThisFrame;
                collider.touchingThisFrame.clear();
            }

            if (colliderArray.m_Components.size() < 2)
                return;

            std::unordered_map<UUID, BodyRuntime> bodies;
            bodies.reserve(colliderArray.m_Components.size());

            for (size_t index = 0; index < colliderArray.m_Components.size(); index++)
            {
                const UUID id = colliderArray.m_IndexToId[index];
                if (!transformArray.m_IdToIndex.count(id))
                    continue;

                BodyRuntime body;
                body.id = id;
                body.collider = &colliderArray.m_Components[index];
                body.transform = &manager.GetComponent<TransformComp>(id, TransformComp::Name);
                body.position = body.transform->GetPosition();
                body.startPosition = body.position;

                if (physicsArray.m_IdToIndex.count(id))
                {
                    body.physics = &manager.GetComponent<PhysicsComp>(id, PhysicsComp::Name);
                    body.physicsDriven = true;
                    body.bodyType = body.physics->bodyType;
                    if (body.bodyType != BodyType::Static)
                    {
                        body.remainingDelta = body.physics->velocity * dt;
                        body.canTranslate = true;
                    }

                    if (movementArray.m_IdToIndex.count(id))
                    {
                        body.movement = &manager.GetComponent<MovementComp>(id, MovementComp::Name);
                        body.movement->m_ProposedDelta = body.remainingDelta;
                    }
                }

                bodies.emplace(id, body);
            }

            if (bodies.size() < 2)
                return;

            SimulateContinuousAABB(manager, bodies);
            ResolveDiscreteCleanup(manager, bodies);

            for (auto& [id, body] : bodies)
            {
                if (!body.physics)
                    continue;

                if (body.movement)
                {
                    body.movement->m_Velocity = body.physics->velocity;
                    body.movement->m_ProposedDelta = body.position - body.startPosition;
                }
            }
        }

        void OnReflect() override
        {
            ImGui::SeparatorText("Collision Bounds");
            ImGui::SliderFloat("Bounds X:", &m_Config.bounds.x, 0.0f, 4000.0f);
            ImGui::SliderFloat("Bounds Y:", &m_Config.bounds.y, 0.0f, 4000.0f);
        }

    private:
        struct BodyRuntime
        {
            UUID id;
            ColliderComp* collider = nullptr;
            TransformComp* transform = nullptr;
            PhysicsComp* physics = nullptr;
            MovementComp* movement = nullptr;
            BodyType bodyType = BodyType::Static;
            bool physicsDriven = false;
            bool canTranslate = false;
            sf::Vector2f startPosition = { 0.0f, 0.0f };
            sf::Vector2f position = { 0.0f, 0.0f };
            sf::Vector2f remainingDelta = { 0.0f, 0.0f };
        };

        struct PairKey
        {
            UUID a;
            UUID b;

            bool operator==(const PairKey& other) const
            {
                return a == other.a && b == other.b;
            }
        };

        struct PairKeyHasher
        {
            std::size_t operator()(const PairKey& key) const noexcept
            {
                const std::size_t h1 = std::hash<UUID>{}(key.a);
                const std::size_t h2 = std::hash<UUID>{}(key.b);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        struct SweepResult
        {
            bool startsTouching = false;
            bool hit = false;
            float toi = 1.0f;
            sf::Vector2f normal = { 0.0f, 0.0f };
        };

        struct Contact
        {
            PairKey pair;
            sf::Vector2f normal = { 0.0f, 0.0f };
        };

    private:
        static constexpr float k_Epsilon = 0.0001f;
        static constexpr float k_TOIEpsilon = 0.0005f;
        static constexpr int k_MaxToiEvents = 32;
        static constexpr int k_MaxContactIterations = 8;
        static constexpr int k_MaxDiscreteIterations = 4;

    private:
        static PairKey MakePairKey(UUID a, UUID b)
        {
            if (b < a)
                std::swap(a, b);
            return PairKey{ a, b };
        }

        static bool IsZeroVector(const sf::Vector2f& value)
        {
            return std::abs(value.x) < k_Epsilon && std::abs(value.y) < k_Epsilon;
        }

        static sf::FloatRect ComputeBoundsAt(const BodyRuntime& body, const sf::Vector2f& position)
        {
            return body.collider->GetWorldBounds(position);
        }

        static sf::FloatRect ComputeSweptBounds(const sf::FloatRect& start, const sf::Vector2f& delta)
        {
            sf::FloatRect end = start;
            end.position += delta;

            const float left = std::min(start.position.x, end.position.x);
            const float top = std::min(start.position.y, end.position.y);
            const float right = std::max(start.position.x + start.size.x, end.position.x + end.size.x);
            const float bottom = std::max(start.position.y + start.size.y, end.position.y + end.size.y);
            return sf::FloatRect({ left, top }, { right - left, bottom - top });
        }

        static bool AABBOverlapOrTouch(const sf::FloatRect& a, const sf::FloatRect& b, float epsilon = k_TOIEpsilon)
        {
            return a.position.x <= b.position.x + b.size.x + epsilon &&
                a.position.x + a.size.x >= b.position.x - epsilon &&
                a.position.y <= b.position.y + b.size.y + epsilon &&
                a.position.y + a.size.y >= b.position.y - epsilon;
        }

        static bool ComputeAABBContact(const sf::FloatRect& boxA, const sf::FloatRect& boxB, sf::Vector2f& normalForA, float& penetration)
        {
            if (!AABBOverlapOrTouch(boxA, boxB))
                return false;

            const float moveLeft = boxB.position.x - (boxA.position.x + boxA.size.x);
            const float moveRight = (boxB.position.x + boxB.size.x) - boxA.position.x;
            const float moveUp = boxB.position.y - (boxA.position.y + boxA.size.y);
            const float moveDown = (boxB.position.y + boxB.size.y) - boxA.position.y;

            const float absLeft = std::abs(moveLeft);
            const float absRight = std::abs(moveRight);
            const float absUp = std::abs(moveUp);
            const float absDown = std::abs(moveDown);

            const float minX = std::min(absLeft, absRight);
            const float minY = std::min(absUp, absDown);

            if (minX <= minY)
            {
                if (absLeft <= absRight)
                {
                    normalForA = { -1.0f, 0.0f };
                    penetration = std::max(0.0f, -moveLeft);
                }
                else
                {
                    normalForA = { 1.0f, 0.0f };
                    penetration = std::max(0.0f, moveRight);
                }
            }
            else
            {
                if (absUp <= absDown)
                {
                    normalForA = { 0.0f, -1.0f };
                    penetration = std::max(0.0f, -moveUp);
                }
                else
                {
                    normalForA = { 0.0f, 1.0f };
                    penetration = std::max(0.0f, moveDown);
                }
            }

            return true;
        }

        static SweepResult SweepAABBPair(const sf::FloatRect& boxA, const sf::Vector2f& deltaA, const sf::FloatRect& boxB, const sf::Vector2f& deltaB)
        {
            SweepResult result;

            sf::Vector2f initialNormal = { 0.0f, 0.0f };
            float initialPenetration = 0.0f;
            if (ComputeAABBContact(boxA, boxB, initialNormal, initialPenetration))
            {
                result.startsTouching = true;
                result.normal = initialNormal;
                if (initialPenetration > k_Epsilon)
                {
                    result.hit = true;
                    result.toi = 0.0f;
                    return result;
                }
            }

            const sf::Vector2f relativeDelta = deltaA - deltaB;
            if (IsZeroVector(relativeDelta))
                return result;

            float xEntry = 0.0f;
            float xExit = 0.0f;
            float yEntry = 0.0f;
            float yExit = 0.0f;

            if (relativeDelta.x > k_Epsilon)
            {
                xEntry = boxB.position.x - (boxA.position.x + boxA.size.x);
                xExit = (boxB.position.x + boxB.size.x) - boxA.position.x;
            }
            else if (relativeDelta.x < -k_Epsilon)
            {
                xEntry = (boxB.position.x + boxB.size.x) - boxA.position.x;
                xExit = boxB.position.x - (boxA.position.x + boxA.size.x);
            }
            else
            {
                if (boxA.position.x + boxA.size.x < boxB.position.x - k_TOIEpsilon || boxB.position.x + boxB.size.x < boxA.position.x - k_TOIEpsilon)
                    return result;
                xEntry = -std::numeric_limits<float>::infinity();
                xExit = std::numeric_limits<float>::infinity();
            }

            if (relativeDelta.y > k_Epsilon)
            {
                yEntry = boxB.position.y - (boxA.position.y + boxA.size.y);
                yExit = (boxB.position.y + boxB.size.y) - boxA.position.y;
            }
            else if (relativeDelta.y < -k_Epsilon)
            {
                yEntry = (boxB.position.y + boxB.size.y) - boxA.position.y;
                yExit = boxB.position.y - (boxA.position.y + boxA.size.y);
            }
            else
            {
                if (boxA.position.y + boxA.size.y < boxB.position.y - k_TOIEpsilon || boxB.position.y + boxB.size.y < boxA.position.y - k_TOIEpsilon)
                    return result;
                yEntry = -std::numeric_limits<float>::infinity();
                yExit = std::numeric_limits<float>::infinity();
            }

            const float txEntry = (std::isinf(xEntry) ? xEntry : xEntry / relativeDelta.x);
            const float txExit = (std::isinf(xExit) ? xExit : xExit / relativeDelta.x);
            const float tyEntry = (std::isinf(yEntry) ? yEntry : yEntry / relativeDelta.y);
            const float tyExit = (std::isinf(yExit) ? yExit : yExit / relativeDelta.y);

            const float tEntry = std::max(txEntry, tyEntry);
            const float tExit = std::min(txExit, tyExit);

            if (!std::isfinite(tEntry) && !result.startsTouching)
                return result;
            if (tEntry > tExit)
                return result;
            if (tExit < 0.0f)
                return result;
            if (tEntry > 1.0f)
                return result;
            if (tEntry <= k_TOIEpsilon)
            {
                if (!result.startsTouching)
                {
                    result.hit = true;
                    result.toi = 0.0f;
                    if (txEntry > tyEntry)
                    {
                        result.normal = (relativeDelta.x > 0.0f) ? sf::Vector2f{ -1.0f, 0.0f } : sf::Vector2f{ 1.0f, 0.0f };
                    }
                    else
                    {
                        result.normal = (relativeDelta.y > 0.0f) ? sf::Vector2f{ 0.0f, -1.0f } : sf::Vector2f{ 0.0f, 1.0f };
                    }
                }
                return result;
            }

            result.hit = true;
            result.toi = std::clamp(tEntry, 0.0f, 1.0f);
            if (txEntry > tyEntry)
            {
                result.normal = (relativeDelta.x > 0.0f) ? sf::Vector2f{ -1.0f, 0.0f } : sf::Vector2f{ 1.0f, 0.0f };
            }
            else
            {
                result.normal = (relativeDelta.y > 0.0f) ? sf::Vector2f{ 0.0f, -1.0f } : sf::Vector2f{ 0.0f, 1.0f };
            }

            return result;
        }

        static void AddTouch(std::vector<UUID>& touched, UUID entity)
        {
            if (std::find(touched.begin(), touched.end(), entity) == touched.end())
                touched.push_back(entity);
        }

        static float InverseMass(const BodyRuntime& body)
        {
            if (!body.physics)
                return 0.0f;
            if (body.physics->bodyType != BodyType::Dynamic)
                return 0.0f;
            if (body.physics->mass <= k_Epsilon)
                return 0.0f;
            return 1.0f / body.physics->mass;
        }

        static sf::Vector2f GetVelocity(const BodyRuntime& body)
        {
            return body.physics ? body.physics->velocity : sf::Vector2f{ 0.0f, 0.0f };
        }

        static void SetVelocity(BodyRuntime& body, const sf::Vector2f& velocity)
        {
            if (!body.physics)
                return;

            body.physics->velocity = velocity;
            if (body.movement)
                body.movement->m_Velocity = velocity;
        }

        bool ApplyContactResponse(BodyRuntime& bodyA, BodyRuntime& bodyB, const sf::Vector2f& normalForA, float penetration)
        {
            bool changed = false;
            const float invMassA = InverseMass(bodyA);
            const float invMassB = InverseMass(bodyB);
            const float totalInvMass = invMassA + invMassB;

            if (penetration > k_Epsilon && totalInvMass > 0.0f)
            {
                const sf::Vector2f correction = normalForA * penetration;
                if (invMassA > 0.0f)
                {
                    const sf::Vector2f moveA = correction * (invMassA / totalInvMass);
                    bodyA.position += moveA;
                    bodyA.transform->SetPosition(bodyA.position);
                    if (bodyA.movement)
                        bodyA.movement->m_WasCorrectedByPhysics = true;
                    changed = true;
                }
                if (invMassB > 0.0f)
                {
                    const sf::Vector2f moveB = correction * (-invMassB / totalInvMass);
                    bodyB.position += moveB;
                    bodyB.transform->SetPosition(bodyB.position);
                    if (bodyB.movement)
                        bodyB.movement->m_WasCorrectedByPhysics = true;
                    changed = true;
                }
            }

            if (totalInvMass <= 0.0f)
                return changed;

            const sf::Vector2f velocityA = GetVelocity(bodyA);
            const sf::Vector2f velocityB = GetVelocity(bodyB);
            const sf::Vector2f relativeVelocity = velocityA - velocityB;
            const float velAlongNormal = relativeVelocity.x * normalForA.x + relativeVelocity.y * normalForA.y;
            if (velAlongNormal >= 0.0f)
                return changed;

            const float restitutionA = bodyA.physics ? PhysicsSystem::ResolveRestitution(*bodyA.physics) : 0.0f;
            const float restitutionB = bodyB.physics ? PhysicsSystem::ResolveRestitution(*bodyB.physics) : 0.0f;
            const float restitution = std::max(restitutionA, restitutionB);

            const float impulseMagnitude = -(1.0f + restitution) * velAlongNormal / totalInvMass;
            const sf::Vector2f impulse = normalForA * impulseMagnitude;

            if (invMassA > 0.0f)
            {
                SetVelocity(bodyA, velocityA + impulse * invMassA);
                changed = true;
            }
            if (invMassB > 0.0f)
            {
                SetVelocity(bodyB, velocityB - impulse * invMassB);
                changed = true;
            }

            const sf::Vector2f postA = GetVelocity(bodyA);
            const sf::Vector2f postB = GetVelocity(bodyB);
            sf::Vector2f tangent = (postA - postB) - normalForA * ((postA - postB).x * normalForA.x + (postA - postB).y * normalForA.y);
            const float tangentLength = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
            if (tangentLength <= k_Epsilon)
                return changed;

            tangent /= tangentLength;
            const float frictionA = bodyA.physics ? PhysicsSystem::ResolveFriction(*bodyA.physics) : 0.0f;
            const float frictionB = bodyB.physics ? PhysicsSystem::ResolveFriction(*bodyB.physics) : 0.0f;
            const float friction = std::max(frictionA, frictionB);
            if (friction <= 0.0f)
                return changed;

            float jt = -((postA - postB).x * tangent.x + (postA - postB).y * tangent.y) / totalInvMass;
            const float maxJt = impulseMagnitude * friction;
            jt = std::clamp(jt, -maxJt, maxJt);
            const sf::Vector2f frictionImpulse = tangent * jt;

            if (invMassA > 0.0f)
                SetVelocity(bodyA, GetVelocity(bodyA) + frictionImpulse * invMassA);
            if (invMassB > 0.0f)
                SetVelocity(bodyB, GetVelocity(bodyB) - frictionImpulse * invMassB);

            return true;
        }

        void SimulateContinuousAABB(EntityManager& manager, std::unordered_map<UUID, BodyRuntime>& bodies)
        {
            sf::Vector2u windowSize = Application::Get().GetWindow().getSize();
            quadtree.BuildTree((m_Config.bounds.x > 0.0f && m_Config.bounds.y > 0.0f)
                ? m_Config.bounds
                : sf::Vector2f{ static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) });

            const auto buildSweptBounds = [&](std::unordered_map<UUID, sf::FloatRect>& sweptBounds)
            {
                sweptBounds.clear();
                sweptBounds.reserve(bodies.size());
                for (const auto& [id, body] : bodies)
                {
                    sf::FloatRect start = ComputeBoundsAt(body, body.position);
                    sf::Vector2f delta = body.canTranslate ? body.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };
                    sweptBounds.emplace(id, ComputeSweptBounds(start, delta));
                }
            };

            std::unordered_map<UUID, sf::FloatRect> sweptBounds;
            buildSweptBounds(sweptBounds);
            quadtree.Populate(manager, &sweptBounds);

            std::vector<PairKey> candidatePairs;
            candidatePairs.reserve(128);
            for (const auto& [a, b] : quadtree.GeneratePairs())
            {
                if (!bodies.count(a) || !bodies.count(b))
                    continue;
                candidatePairs.push_back(MakePairKey(a, b));
            }

            for (const auto& [id, body] : bodies)
            {
                for (const UUID touchId : body.collider->touchingLastFrame)
                {
                    if (!bodies.count(touchId) || touchId == id)
                        continue;
                    candidatePairs.push_back(MakePairKey(id, touchId));
                }
            }

            if (candidatePairs.empty())
            {
                for (auto& [id, body] : bodies)
                {
                    if (!body.canTranslate)
                        continue;
                    body.position += body.remainingDelta;
                    body.transform->SetPosition(body.position);
                    body.remainingDelta = { 0.0f, 0.0f };
                }
                return;
            }

            std::unordered_map<UUID, UUID> parent;
            parent.reserve(bodies.size());
            for (const auto& [id, body] : bodies)
                parent[id] = id;

            const auto findRoot = [&](UUID id, auto&& self) -> UUID
            {
                UUID root = id;
                while (!(parent[root] == root))
                    root = parent[root];

                UUID current = id;
                while (!(parent[current] == current))
                {
                    UUID next = parent[current];
                    parent[current] = root;
                    current = next;
                }
                return root;
            };

            auto unite = [&](UUID a, UUID b)
            {
                UUID rootA = findRoot(a, findRoot);
                UUID rootB = findRoot(b, findRoot);
                if (rootA == rootB)
                    return;
                parent[rootB] = rootA;
            };

            for (const PairKey& pair : candidatePairs)
                unite(pair.a, pair.b);

            std::unordered_map<UUID, std::vector<PairKey>> islandPairs;
            for (const PairKey& pair : candidatePairs)
            {
                const UUID root = findRoot(pair.a, findRoot);
                islandPairs[root].push_back(pair);
            }

            std::unordered_set<UUID> bodiesInIslands;
            for (auto& [root, pairs] : islandPairs)
            {
                std::unordered_set<UUID> islandBodies;
                for (const PairKey& pair : pairs)
                {
                    islandBodies.insert(pair.a);
                    islandBodies.insert(pair.b);
                }
                bodiesInIslands.insert(islandBodies.begin(), islandBodies.end());

                int zeroProgressGuard = 0;
                for (int eventIndex = 0; eventIndex < k_MaxToiEvents; eventIndex++)
                {
                    float earliestToi = 1.0f;
                    std::vector<Contact> earliestContacts;
                    std::vector<Contact> persistentContacts;

                    for (const PairKey& pair : pairs)
                    {
                        BodyRuntime& bodyA = bodies[pair.a];
                        BodyRuntime& bodyB = bodies[pair.b];

                        if (bodyA.collider->GetType() != ColliderType::AABB || bodyB.collider->GetType() != ColliderType::AABB)
                            continue;

                        const sf::FloatRect boundsA = ComputeBoundsAt(bodyA, bodyA.position);
                        const sf::FloatRect boundsB = ComputeBoundsAt(bodyB, bodyB.position);
                        const sf::Vector2f deltaA = bodyA.canTranslate ? bodyA.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };
                        const sf::Vector2f deltaB = bodyB.canTranslate ? bodyB.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };

                        SweepResult sweep = SweepAABBPair(boundsA, deltaA, boundsB, deltaB);
                        if (sweep.startsTouching)
                        {
                            persistentContacts.push_back(Contact{ pair, sweep.normal });
                        }

                        if (!sweep.hit)
                            continue;

                        if (sweep.toi + k_TOIEpsilon < earliestToi)
                        {
                            earliestToi = sweep.toi;
                            earliestContacts.clear();
                        }

                        if (std::abs(sweep.toi - earliestToi) <= k_TOIEpsilon)
                        {
                            earliestContacts.push_back(Contact{ pair, sweep.normal });
                        }
                    }

                    const bool hasEarliest = !earliestContacts.empty();
                    const bool hasPersistent = !persistentContacts.empty();
                    if (!hasEarliest && !hasPersistent)
                    {
                        bool moved = false;
                        for (UUID bodyId : islandBodies)
                        {
                            BodyRuntime& body = bodies[bodyId];
                            if (!body.canTranslate)
                                continue;
                            if (IsZeroVector(body.remainingDelta))
                                continue;

                            body.position += body.remainingDelta;
                            body.transform->SetPosition(body.position);
                            body.remainingDelta = { 0.0f, 0.0f };
                            moved = true;
                        }

                        if (!moved)
                            break;

                        continue;
                    }

                    float advance = hasEarliest ? earliestToi : 0.0f;
                    advance = std::clamp(advance, 0.0f, 1.0f);
                    if (advance > k_TOIEpsilon)
                    {
                        for (UUID bodyId : islandBodies)
                        {
                            BodyRuntime& body = bodies[bodyId];
                            if (!body.canTranslate)
                                continue;

                            const sf::Vector2f deltaStep = body.remainingDelta * advance;
                            body.position += deltaStep;
                            body.transform->SetPosition(body.position);
                            body.remainingDelta -= deltaStep;
                        }
                    }

                    std::unordered_map<PairKey, Contact, PairKeyHasher> batch;
                    for (const Contact& contact : persistentContacts)
                    {
                        batch[contact.pair] = contact;
                    }
                    for (const Contact& contact : earliestContacts)
                    {
                        batch[contact.pair] = contact;
                    }

                    for (const PairKey& pair : pairs)
                    {
                        BodyRuntime& bodyA = bodies[pair.a];
                        BodyRuntime& bodyB = bodies[pair.b];
                        if (bodyA.collider->GetType() != ColliderType::AABB || bodyB.collider->GetType() != ColliderType::AABB)
                            continue;

                        sf::Vector2f normal = { 0.0f, 0.0f };
                        float penetration = 0.0f;
                        if (ComputeAABBContact(ComputeBoundsAt(bodyA, bodyA.position), ComputeBoundsAt(bodyB, bodyB.position), normal, penetration))
                        {
                            batch[pair] = Contact{ pair, normal };
                        }
                    }

                    bool eventProgress = false;
                    for (int iteration = 0; iteration < k_MaxContactIterations; iteration++)
                    {
                        bool iterationProgress = false;
                        for (auto& [pair, contact] : batch)
                        {
                            BodyRuntime& bodyA = bodies[pair.a];
                            BodyRuntime& bodyB = bodies[pair.b];

                            sf::Vector2f normal = contact.normal;
                            float penetration = 0.0f;
                            if (!ComputeAABBContact(ComputeBoundsAt(bodyA, bodyA.position), ComputeBoundsAt(bodyB, bodyB.position), normal, penetration))
                                continue;

                            AddTouch(bodyA.collider->touchingThisFrame, bodyB.id);
                            AddTouch(bodyB.collider->touchingThisFrame, bodyA.id);

                            if (ApplyContactResponse(bodyA, bodyB, normal, penetration))
                            {
                                iterationProgress = true;
                            }
                        }

                        eventProgress |= iterationProgress;
                        if (!iterationProgress)
                            break;
                    }

                    if (advance <= k_TOIEpsilon && !eventProgress)
                    {
                        zeroProgressGuard++;
                        if (zeroProgressGuard >= 2)
                        {
                            break;
                        }
                    }
                    else
                    {
                        zeroProgressGuard = 0;
                    }

                    bool islandDone = true;
                    for (UUID bodyId : islandBodies)
                    {
                        if (!IsZeroVector(bodies[bodyId].remainingDelta))
                        {
                            islandDone = false;
                            break;
                        }
                    }
                    if (islandDone)
                        break;
                }
            }

            for (auto& [id, body] : bodies)
            {
                if (!body.canTranslate)
                    continue;
                if (bodiesInIslands.count(id))
                    continue;
                if (IsZeroVector(body.remainingDelta))
                    continue;

                body.position += body.remainingDelta;
                body.transform->SetPosition(body.position);
                body.remainingDelta = { 0.0f, 0.0f };
            }
        }

        bool ComputePairCorrection(EntityManager& manager, std::unordered_map<UUID, BodyRuntime>& bodies, UUID entityA, UUID entityB, sf::Vector2f& correctionForA)
        {
            if (!bodies.count(entityA) || !bodies.count(entityB))
                return false;

            BodyRuntime& bodyA = bodies[entityA];
            BodyRuntime& bodyB = bodies[entityB];

            const sf::FloatRect boundsA = ComputeBoundsAt(bodyA, bodyA.position);
            const sf::FloatRect boundsB = ComputeBoundsAt(bodyB, bodyB.position);
            if (!AABBOverlapOrTouch(boundsA, boundsB, 0.0f))
                return false;

            const ColliderType typeA = bodyA.collider->GetType();
            const ColliderType typeB = bodyB.collider->GetType();

            bool collided = false;
            if (typeA == ColliderType::AABB && typeB == ColliderType::AABB)
            {
                sf::Vector2f normal = { 0.0f, 0.0f };
                float penetration = 0.0f;
                if (ComputeAABBContact(boundsA, boundsB, normal, penetration) && penetration > k_Epsilon)
                {
                    correctionForA = normal * penetration;
                    collided = true;
                }
            }
            else if (typeA == ColliderType::Circle && typeB == ColliderType::Circle)
            {
                const sf::Vector2f posA = bodyA.collider->GetWorldOrigin(bodyA.position);
                const sf::Vector2f posB = bodyB.collider->GetWorldOrigin(bodyB.position);
                const auto* circleA = bodyA.collider->AsCircle();
                const auto* circleB = bodyB.collider->AsCircle();
                if (circleA && circleB)
                    collided = IntersectCircleCircle(posA, circleA->radius, posB, circleB->radius, correctionForA);
            }
            else if (typeA == ColliderType::AABB && typeB == ColliderType::Circle)
            {
                const sf::Vector2f posB = bodyB.collider->GetWorldOrigin(bodyB.position);
                const auto* circleB = bodyB.collider->AsCircle();
                if (circleB)
                    collided = IntersectAABBCircle(boundsA, posB, circleB->radius, correctionForA);
            }
            else if (typeA == ColliderType::Circle && typeB == ColliderType::AABB)
            {
                const sf::Vector2f posA = bodyA.collider->GetWorldOrigin(bodyA.position);
                const auto* circleA = bodyA.collider->AsCircle();
                sf::Vector2f correctionForB = { 0.0f, 0.0f };
                if (circleA && IntersectAABBCircle(boundsB, posA, circleA->radius, correctionForB))
                {
                    correctionForA = { -correctionForB.x, -correctionForB.y };
                    collided = true;
                }
            }

            if (!collided)
                return false;

            AddTouch(bodyA.collider->touchingThisFrame, entityB);
            AddTouch(bodyB.collider->touchingThisFrame, entityA);
            return true;
        }

        static bool IntersectCircleCircle(const sf::Vector2f& centerA, float radiusA, const sf::Vector2f& centerB, float radiusB, sf::Vector2f& correctionForA)
        {
            const sf::Vector2f delta = centerA - centerB;
            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            const float combinedRadius = radiusA + radiusB;
            const float combinedRadiusSquared = combinedRadius * combinedRadius;
            if (distanceSquared >= combinedRadiusSquared)
                return false;

            const float distance = std::sqrt(distanceSquared);
            if (distance > k_Epsilon)
            {
                const sf::Vector2f normal = delta / distance;
                correctionForA = normal * (combinedRadius - distance);
            }
            else
            {
                correctionForA = { combinedRadius, 0.0f };
            }

            return !IsZeroVector(correctionForA);
        }

        static bool IntersectAABBCircle(const sf::FloatRect& boxA, const sf::Vector2f& circleCenter, float circleRadius, sf::Vector2f& correctionForA)
        {
            const float closestX = std::clamp(circleCenter.x, boxA.position.x, boxA.position.x + boxA.size.x);
            const float closestY = std::clamp(circleCenter.y, boxA.position.y, boxA.position.y + boxA.size.y);
            sf::Vector2f delta = { closestX - circleCenter.x, closestY - circleCenter.y };

            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            const float radiusSquared = circleRadius * circleRadius;
            if (distanceSquared >= radiusSquared)
                return false;

            if (distanceSquared > k_Epsilon)
            {
                const float distance = std::sqrt(distanceSquared);
                const sf::Vector2f normal = delta / distance;
                const sf::Vector2f correctionForCircle = normal * (circleRadius - distance);
                correctionForA = { -correctionForCircle.x, -correctionForCircle.y };
                return !IsZeroVector(correctionForA);
            }

            const sf::Vector2f center = {
                boxA.position.x + boxA.size.x * 0.5f,
                boxA.position.y + boxA.size.y * 0.5f
            };
            const sf::Vector2f fromCenter = circleCenter - center;
            const float overlapX = (boxA.size.x * 0.5f + circleRadius) - std::abs(fromCenter.x);
            const float overlapY = (boxA.size.y * 0.5f + circleRadius) - std::abs(fromCenter.y);
            if (overlapX <= overlapY)
                correctionForA = { fromCenter.x < 0.0f ? overlapX : -overlapX, 0.0f };
            else
                correctionForA = { 0.0f, fromCenter.y < 0.0f ? overlapY : -overlapY };

            return !IsZeroVector(correctionForA);
        }

        void ResolveDiscreteCleanup(EntityManager& manager, std::unordered_map<UUID, BodyRuntime>& bodies)
        {
            for (int iteration = 0; iteration < k_MaxDiscreteIterations; iteration++)
            {
                bool appliedCorrection = false;

                sf::Vector2u windowSize = Application::Get().GetWindow().getSize();
                quadtree.BuildTree((m_Config.bounds.x > 0.0f && m_Config.bounds.y > 0.0f)
                    ? m_Config.bounds
                    : sf::Vector2f{ static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) });

                std::unordered_map<UUID, sf::FloatRect> currentBounds;
                currentBounds.reserve(bodies.size());
                for (const auto& [id, body] : bodies)
                {
                    currentBounds.emplace(id, ComputeBoundsAt(body, body.position));
                }
                quadtree.Populate(manager, &currentBounds);

                for (const auto& [entityA, entityB] : quadtree.GeneratePairs())
                {
                    sf::Vector2f correctionForA = { 0.0f, 0.0f };
                    if (!ComputePairCorrection(manager, bodies, entityA, entityB, correctionForA))
                        continue;

                    BodyRuntime& bodyA = bodies[entityA];
                    BodyRuntime& bodyB = bodies[entityB];
                    sf::Vector2f normal = { 0.0f, 0.0f };
                    float penetration = std::sqrt(correctionForA.x * correctionForA.x + correctionForA.y * correctionForA.y);
                    if (penetration > k_Epsilon)
                    {
                        normal = correctionForA / penetration;
                        appliedCorrection |= ApplyContactResponse(bodyA, bodyB, normal, penetration);
                    }
                }

                if (!appliedCorrection)
                    break;
            }
        }

        Quadtree quadtree;
        CollisionSystemConfig m_Config;
    };
}
