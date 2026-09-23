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
        float remainingTime = 0.0f;
        sf::Vector2f remainingDelta = { 0.0f, 0.0f };
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
        std::pair<UUID, UUID> pair;
        sf::Vector2f normal = { 0.0f, 0.0f };
    };

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

        void Update(sf::Time tick, EntityManager& manager) override;

        void OnReflect() override
        {
            ImGui::SeparatorText("Collision Bounds");
            ImGui::SliderFloat("Bounds X:", &m_Config.bounds.x, 0.0f, 4000.0f);
            ImGui::SliderFloat("Bounds Y:", &m_Config.bounds.y, 0.0f, 4000.0f);
        }        

    private:
        static constexpr float k_Epsilon = 0.0001f;
        static constexpr float k_TOIEpsilon = 0.0005f;
        static constexpr int k_MaxToiEvents = 32;
        static constexpr int k_MaxContactIterations = 8;
        static constexpr int k_MaxDiscreteIterations = 4;

    private:
        bool IsZeroVector(const sf::Vector2f& value);
        sf::FloatRect ComputeBoundsAt(const BodyRuntime& body, const sf::Vector2f& position);
        sf::FloatRect ComputeSweptBounds(const sf::FloatRect& start, const sf::Vector2f& delta);
        bool AABBOverlapOrTouch(const sf::FloatRect& a, const sf::FloatRect& b, float epsilon = k_TOIEpsilon);
        bool ComputeAABBContact(const sf::FloatRect& boxA, const sf::FloatRect& boxB,
                                sf::Vector2f& normalForA, float& penetration);
        SweepResult SweepAABBPair(const sf::FloatRect& boxA, const sf::Vector2f& deltaA, 
                                  const sf::FloatRect& boxB, const sf::Vector2f& deltaB);
        void AddTouch(std::vector<UUID>& touched, UUID entity);
        float InverseMass(const BodyRuntime& body);
        sf::Vector2f GetVelocity(const BodyRuntime& body);
        void SetVelocity(BodyRuntime& body, const sf::Vector2f& velocity);
        void RecomputeRemainingDelta(BodyRuntime& body);
        void SyncVelocityFromRemainingDelta(BodyRuntime& body);
        void CommitRemainingMotion(BodyRuntime& body);
        void SyncMovementState(BodyRuntime& body);
        bool ConstrainPersistentContactMotion(BodyRuntime& bodyA, BodyRuntime& bodyB,
                                              const sf::Vector2f& normalForA);
        bool ApplyContactResponse(BodyRuntime& bodyA, BodyRuntime& bodyB,
                                  const sf::Vector2f& normalForA, float penetration);
        void SimulateContinuousAABB(std::unordered_map<UUID, BodyRuntime>& bodies);
        bool ComputePairCorrection(std::unordered_map<UUID, BodyRuntime>& bodies, 
                                   UUID entityA, UUID entityB, sf::Vector2f& correctionForA);

        bool IntersectCircleCircle(const sf::Vector2f& centerA, float radiusA,
                                   const sf::Vector2f& centerB, float radiusB, sf::Vector2f& correctionForA);
        bool IntersectAABBCircle(const sf::FloatRect& boxA, const sf::Vector2f& circleCenter, 
                                 float circleRadius, sf::Vector2f& correctionForA);
        void ResolveDiscreteCleanup(std::unordered_map<UUID, BodyRuntime>& bodies);
        float DotProd(const sf::Vector2f& a, const sf::Vector2f& b);

    private:
        Quadtree quadtree;
        CollisionSystemConfig m_Config;
    };
}
