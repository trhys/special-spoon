#pragma once

#include "System/System.h"
#include "FrameMotion.h"
#include "PhysicsSystem.h"
#include "QuadTree.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"

#include <optional>
#include <cmath>
#include <algorithm>
#include <array>
#include <limits>
#include <unordered_map>
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

            auto& colliderArray = manager.GetArray<ColliderComp>(ColliderComp::Name);
            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            for (auto& movementComp : movementArray.m_Components)
            {
                movementComp.m_WasCorrectedByPhysics = false;
            }

            for (auto& collider : colliderArray.m_Components)
            {
                collider.touchingLastFrame = collider.touchingThisFrame;
                collider.touchingThisFrame.clear();
            }

            m_FrameMotionDt = dt;
            m_FrameMotionCache.clear();

            if (colliderArray.m_Components.size() < 2)
                return;

            ResolveSweptCollisions(manager, dt);

            constexpr int maxIterations = 8;
            for (int iteration = 0; iteration < maxIterations; iteration++)
            {
                bool appliedCorrection = false;
                sf::Vector2u windowSize = Application::Get().GetWindow().getSize();
                quadtree.BuildTree((m_Config.bounds.x > 0.0f && m_Config.bounds.y > 0.0f) ? m_Config.bounds : sf::Vector2f{ static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) });
                quadtree.Populate(manager);

                for (const auto& [entityA, entityB] : quadtree.GeneratePairs())
                {
                    sf::Vector2f correctionForA = { 0.0f, 0.0f };
                    if (!ComputePairCorrection(manager, entityA, entityB, correctionForA))
                        continue;

                    appliedCorrection |= ResolvePair(manager, entityA, entityB, correctionForA);
                }

                if (!appliedCorrection)
                    break;
            }
        }

        void OnReflect() override {
          ImGui::SeparatorText("Collision Bounds");
          ImGui::SliderFloat("Bounds X:", &m_Config.bounds.x, 0.0f, 4000.0f);
          ImGui::SliderFloat("Bounds Y:", &m_Config.bounds.y, 0.0f, 4000.0f);
        }

    private:
        struct SweptAABBHit
        {
           float time = 1.0f;
           sf::Vector2f normal = { 0.0f, 0.0f };
        };

        struct SweptPairHit
        {
           UUID entityA;
           UUID entityB;
           float time = 1.0f;
           sf::Vector2f normal = { 0.0f, 0.0f };
        };

         bool IsZeroVector(const sf::Vector2f& value)
        {
           return std::abs(value.x) < 0.0001f && std::abs(value.y) < 0.0001f;
        }

         std::optional<SweptAABBHit> SweptAABB(const sf::FloatRect& movingBox, const sf::Vector2f& delta, const sf::FloatRect& targetBox)
        {
           if (IsZeroVector(delta))
               return std::nullopt;

           const auto buildInterval = [](float movingMin, float movingMax, float targetMin, float targetMax, float axisDelta, float& entry, float& exit)
           {
               if (std::abs(axisDelta) < 0.0001f)
               {
                   if (movingMax <= targetMin || movingMin >= targetMax)
                       return false;

                   entry = -std::numeric_limits<float>::infinity();
                   exit = std::numeric_limits<float>::infinity();
                   return true;
               }

               if (axisDelta > 0.0f)
               {
                   entry = (targetMin - movingMax) / axisDelta;
                   exit = (targetMax - movingMin) / axisDelta;
               }
               else
               {
                   entry = (targetMax - movingMin) / axisDelta;
                   exit = (targetMin - movingMax) / axisDelta;
               }

               return true;
           };

           float xEntry = 0.0f;
           float xExit = 0.0f;
           if (!buildInterval(
               movingBox.position.x,
               movingBox.position.x + movingBox.size.x,
               targetBox.position.x,
               targetBox.position.x + targetBox.size.x,
               delta.x,
               xEntry,
               xExit))
           {
               return std::nullopt;
           }

           float yEntry = 0.0f;
           float yExit = 0.0f;
           if (!buildInterval(
               movingBox.position.y,
               movingBox.position.y + movingBox.size.y,
               targetBox.position.y,
               targetBox.position.y + targetBox.size.y,
               delta.y,
               yEntry,
               yExit))
           {
               return std::nullopt;
           }

           const float entryTime = std::max(xEntry, yEntry);
           const float exitTime = std::min(xExit, yExit);
           if (entryTime < -0.0001f || entryTime > exitTime || exitTime < 0.0f || entryTime > 1.0f)
               return std::nullopt;

           SweptAABBHit hit;
           hit.time = std::max(0.0f, entryTime);
           if (xEntry > yEntry)
               hit.normal = delta.x > 0.0f ? sf::Vector2f{ -1.0f, 0.0f } : sf::Vector2f{ 1.0f, 0.0f };
           else
               hit.normal = delta.y > 0.0f ? sf::Vector2f{ 0.0f, -1.0f } : sf::Vector2f{ 0.0f, 1.0f };

           return hit;
        }

         std::optional<SweptAABBHit> SweptCircleCircle(const sf::Vector2f& centerA, float radiusA, const sf::Vector2f& delta, const sf::Vector2f& centerB, float radiusB)
        {
           const sf::Vector2f startOffset = centerA - centerB;
           const float combinedRadius = radiusA + radiusB;
           const float c = startOffset.x * startOffset.x + startOffset.y * startOffset.y - combinedRadius * combinedRadius;
           if (c <= 0.0001f)
              return std::nullopt;

           const float a = delta.x * delta.x + delta.y * delta.y;
           if (a <= 0.0001f)
              return std::nullopt;

           const float b = 2.0f * (startOffset.x * delta.x + startOffset.y * delta.y);
           if (b >= -0.0001f)
              return std::nullopt;

           const float discriminant = b * b - 4.0f * a * c;
           if (discriminant < 0.0f)
              return std::nullopt;

           const float impactTime = (-b - std::sqrt(discriminant)) / (2.0f * a);
           if (impactTime < -0.0001f || impactTime > 1.0f)
              return std::nullopt;

           const sf::Vector2f impactOffset = startOffset + delta * std::max(0.0f, impactTime);
           const float impactDistance = std::sqrt(impactOffset.x * impactOffset.x + impactOffset.y * impactOffset.y);
           if (impactDistance <= 0.0001f)
              return std::nullopt;

           return SweptAABBHit{
              std::max(0.0f, impactTime),
              impactOffset / impactDistance
           };
        }

         std::optional<SweptAABBHit> SweptCircleAABB(const sf::Vector2f& center, float radius, const sf::Vector2f& delta, const sf::FloatRect& box)
        {
           std::optional<SweptAABBHit> bestHit;
           auto considerHit = [&bestHit](float time, const sf::Vector2f& normal)
           {
              if (time < -0.0001f || time > 1.0f)
                  return;
              if (!bestHit || time < bestHit->time)
                  bestHit = SweptAABBHit{ std::max(0.0f, time), normal };
           };

           const float left = box.position.x;
           const float right = box.position.x + box.size.x;
           const float top = box.position.y;
           const float bottom = box.position.y + box.size.y;

           if (delta.x > 0.0001f && center.x + radius <= left)
           {
              const float impactTime = (left - (center.x + radius)) / delta.x;
              const float impactY = center.y + delta.y * impactTime;
              if (impactY >= top && impactY <= bottom)
                  considerHit(impactTime, { -1.0f, 0.0f });
           }
           else if (delta.x < -0.0001f && center.x - radius >= right)
           {
              const float impactTime = (right - (center.x - radius)) / delta.x;
              const float impactY = center.y + delta.y * impactTime;
              if (impactY >= top && impactY <= bottom)
                  considerHit(impactTime, { 1.0f, 0.0f });
           }

           if (delta.y > 0.0001f && center.y + radius <= top)
           {
              const float impactTime = (top - (center.y + radius)) / delta.y;
              const float impactX = center.x + delta.x * impactTime;
              if (impactX >= left && impactX <= right)
                  considerHit(impactTime, { 0.0f, -1.0f });
           }
           else if (delta.y < -0.0001f && center.y - radius >= bottom)
           {
              const float impactTime = (bottom - (center.y - radius)) / delta.y;
              const float impactX = center.x + delta.x * impactTime;
              if (impactX >= left && impactX <= right)
                  considerHit(impactTime, { 0.0f, 1.0f });
           }

           const std::array<sf::Vector2f, 4> corners = {
              sf::Vector2f{ left, top },
              sf::Vector2f{ right, top },
              sf::Vector2f{ left, bottom },
              sf::Vector2f{ right, bottom }
           };
           for (const sf::Vector2f& corner : corners)
           {
              if (std::optional<SweptAABBHit> cornerHit = SweptCircleCircle(center, radius, delta, corner, 0.0f))
                  considerHit(cornerHit->time, cornerHit->normal);
           }

           return bestHit;
        }

         bool ComputePairCorrection(const ColliderComp& colliderA, const sf::Vector2f& transformPositionA, const ColliderComp& colliderB, const sf::Vector2f& transformPositionB, sf::Vector2f& correctionForA)
        {
           const sf::Vector2f posA = colliderA.GetWorldOrigin(transformPositionA);
           const sf::Vector2f posB = colliderB.GetWorldOrigin(transformPositionB);
           const sf::FloatRect boundsA = colliderA.GetWorldBounds(transformPositionA);
           const sf::FloatRect boundsB = colliderB.GetWorldBounds(transformPositionB);
           if (!boundsA.findIntersection(boundsB))
              return false;

           const ColliderType typeA = colliderA.GetType();
           const ColliderType typeB = colliderB.GetType();

           bool collided = false;
           if (typeA == ColliderType::AABB && typeB == ColliderType::AABB)
           {
              correctionForA = ComputeAABBCorrection(boundsA, boundsB);
              collided = !IsZeroVector(correctionForA);
           }
           else if (typeA == ColliderType::Circle && typeB == ColliderType::Circle)
           {
              const auto* circleA = colliderA.AsCircle();
              const auto* circleB = colliderB.AsCircle();
              if (circleA && circleB)
                  collided = IntersectCircleCircle(posA, circleA->radius, posB, circleB->radius, correctionForA);
           }
           else if (typeA == ColliderType::AABB && typeB == ColliderType::Circle)
           {
              const auto* circleB = colliderB.AsCircle();
              if (circleB)
                  collided = IntersectAABBCircle(boundsA, posB, circleB->radius, correctionForA);
           }
           else if (typeA == ColliderType::Circle && typeB == ColliderType::AABB)
           {
              const auto* circleA = colliderA.AsCircle();
              sf::Vector2f correctionForB = { 0.0f, 0.0f };
              if (circleA && IntersectAABBCircle(boundsB, posA, circleA->radius, correctionForB))
              {
                  correctionForA = { -correctionForB.x, -correctionForB.y };
                  collided = true;
              }
           }

           return collided;
        }

         std::optional<SweptPairHit> ComputeSweptPairHit(EntityManager& manager, UUID entityA, UUID entityB, float dt)
        {
           auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
           auto& colliderArray = manager.GetArray<ColliderComp>(ColliderComp::Name);
           if (!transformArray.m_IdToIndex.count(entityA) || !transformArray.m_IdToIndex.count(entityB))
               return std::nullopt;
           if (!colliderArray.m_IdToIndex.count(entityA) || !colliderArray.m_IdToIndex.count(entityB))
               return std::nullopt;

           auto& colliderA = manager.GetComponent<ColliderComp>(entityA, ColliderComp::Name);
           auto& colliderB = manager.GetComponent<ColliderComp>(entityB, ColliderComp::Name);

           sf::Vector2f correctionForA = { 0.0f, 0.0f };
           const FrameMotion motionA = GetFrameMotion(manager, entityA);
           const FrameMotion motionB = GetFrameMotion(manager, entityB);

           const sf::Vector2f deltaA = motionA.delta;
           const sf::Vector2f deltaB = motionB.delta;
           if (IsZeroVector(deltaA) && IsZeroVector(deltaB))
               return std::nullopt;

           const sf::Vector2f startTransformA = motionA.startPosition;
           const sf::Vector2f startTransformB = motionB.startPosition;
           if (ComputePairCorrection(colliderA, startTransformA, colliderB, startTransformB, correctionForA))
               return std::nullopt;

           const sf::Vector2f relativeDelta = deltaA - deltaB;
           const sf::FloatRect startBoundsA = colliderA.GetWorldBounds(startTransformA);
           const sf::FloatRect startBoundsB = colliderB.GetWorldBounds(startTransformB);
           const sf::Vector2f startOriginA = colliderA.GetWorldOrigin(startTransformA);
           const sf::Vector2f startOriginB = colliderB.GetWorldOrigin(startTransformB);

           std::optional<SweptAABBHit> hit;
           if (colliderA.GetType() == ColliderType::AABB && colliderB.GetType() == ColliderType::AABB)
           {
               hit = SweptAABB(startBoundsA, relativeDelta, startBoundsB);
           }
           else if (colliderA.GetType() == ColliderType::Circle && colliderB.GetType() == ColliderType::Circle)
           {
               const auto* circleA = colliderA.AsCircle();
               const auto* circleB = colliderB.AsCircle();
               if (circleA && circleB)
                   hit = SweptCircleCircle(startOriginA, circleA->radius, relativeDelta, startOriginB, circleB->radius);
           }
           else if (colliderA.GetType() == ColliderType::Circle && colliderB.GetType() == ColliderType::AABB)
           {
               const auto* circleA = colliderA.AsCircle();
               if (circleA)
                   hit = SweptCircleAABB(startOriginA, circleA->radius, relativeDelta, startBoundsB);
           }
           else if (colliderA.GetType() == ColliderType::AABB && colliderB.GetType() == ColliderType::Circle)
           {
               const auto* circleB = colliderB.AsCircle();
               if (circleB)
               {
                   hit = SweptCircleAABB(
                       startOriginB,
                       circleB->radius,
                       sf::Vector2f{ -relativeDelta.x, -relativeDelta.y },
                       startBoundsA);
                   if (hit)
                       hit->normal = { -hit->normal.x, -hit->normal.y };
               }
           }

           if (!hit)
               return std::nullopt;

           return SweptPairHit{
               entityA,
               entityB,
               hit->time,
               hit->normal
           };
        }

         bool StartedOverlapping(EntityManager& manager, UUID entityA, UUID entityB, float dt)
        {
           auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
           auto& colliderArray = manager.GetArray<ColliderComp>(ColliderComp::Name);
           if (!transformArray.m_IdToIndex.count(entityA) || !transformArray.m_IdToIndex.count(entityB))
               return false;
           if (!colliderArray.m_IdToIndex.count(entityA) || !colliderArray.m_IdToIndex.count(entityB))
               return false;

           auto& colliderA = manager.GetComponent<ColliderComp>(entityA, ColliderComp::Name);
           auto& colliderB = manager.GetComponent<ColliderComp>(entityB, ColliderComp::Name);
           const FrameMotion& motionA = GetFrameMotion(manager, entityA);
           const FrameMotion& motionB = GetFrameMotion(manager, entityB);
           sf::Vector2f correctionForA = { 0.0f, 0.0f };
           return ComputePairCorrection(
               colliderA,
               motionA.startPosition,
               colliderB,
               motionB.startPosition,
               correctionForA);
        }

         bool ApplySweepClamp(EntityManager& manager, UUID entity, const FrameMotion& motion, const sf::Vector2f& normal, float time)
        {
           const sf::Vector2f& fullDelta = motion.delta;
           if (IsZeroVector(fullDelta))
               return false;

           const float clampedTime = std::clamp(time, 0.0f, 1.0f);
           const bool corrected = clampedTime < 0.9999f;
           if (!corrected)
               return false;

           const float motionAlongNormal = fullDelta.x * normal.x + fullDelta.y * normal.y;
           const sf::Vector2f normalDelta = normal * motionAlongNormal;
           const sf::Vector2f tangentialDelta = fullDelta - normalDelta;
           const sf::Vector2f appliedDelta = tangentialDelta + normalDelta * clampedTime;
           const sf::Vector2f correction = appliedDelta - fullDelta;
           const sf::Vector2f transformDelta = motion.transformAlreadyAdvanced ? correction : appliedDelta;
           sf::Vector2f updatedPosition = motion.currentPosition;

            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            if (transformArray.m_IdToIndex.count(entity))
            {
               auto& transform = manager.GetComponent<TransformComp>(entity, TransformComp::Name);
               if (!IsZeroVector(transformDelta))
                   transform.Move(transformDelta);
               updatedPosition = transform.GetPosition();
            }
           else if (!IsZeroVector(transformDelta))
           {
               updatedPosition += transformDelta;
           }

           auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
           if (movementArray.m_IdToIndex.count(entity))
           {
               auto& movement = manager.GetComponent<MovementComp>(entity, MovementComp::Name);
               movement.m_ProposedDelta = appliedDelta;
               movement.m_WasCorrectedByPhysics = true;
           }

           m_FrameMotionCache[entity] = FrameMotion{
               updatedPosition,
               updatedPosition,
               { 0.0f, 0.0f },
               false
           };

           return true;
        }

         void ResolveSweptCollisions(EntityManager& manager, float dt)
        {
           if (dt <= 0.0f)
               return;

           sf::Vector2u windowSize = Application::Get().GetWindow().getSize();
           quadtree.BuildTree((m_Config.bounds.x > 0.0f && m_Config.bounds.y > 0.0f) ? m_Config.bounds : sf::Vector2f{ static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) });
           quadtree.PopulateSwept(manager, dt);

           std::vector<SweptPairHit> hits;
           for (const auto& [entityA, entityB] : quadtree.GeneratePairs())
           {
               if (StartedOverlapping(manager, entityA, entityB, dt))
               {
                   auto& colliderA = manager.GetComponent<ColliderComp>(entityA, ColliderComp::Name);
                   auto& colliderB = manager.GetComponent<ColliderComp>(entityB, ColliderComp::Name);
                   AddTouch(colliderA.touchingThisFrame, entityB);
                   AddTouch(colliderB.touchingThisFrame, entityA);
               }

               if (std::optional<SweptPairHit> hit = ComputeSweptPairHit(manager, entityA, entityB, dt))
                   hits.push_back(*hit);
           }

           std::sort(hits.begin(), hits.end(), [](const SweptPairHit& lhs, const SweptPairHit& rhs)
           {
               return lhs.time < rhs.time;
           });

           for (const SweptPairHit& cachedHit : hits)
           {
               std::optional<SweptPairHit> hit = ComputeSweptPairHit(manager, cachedHit.entityA, cachedHit.entityB, dt);
               if (!hit)
                   continue;

               const FrameMotion& motionA = GetFrameMotion(manager, hit->entityA);
               const FrameMotion& motionB = GetFrameMotion(manager, hit->entityB);
               const float normalMotionA = motionA.delta.x * hit->normal.x + motionA.delta.y * hit->normal.y;
               const sf::Vector2f inverseNormal = { -hit->normal.x, -hit->normal.y };
               const float normalMotionB = motionB.delta.x * inverseNormal.x + motionB.delta.y * inverseNormal.y;
               const bool movedA = normalMotionA < -0.0001f && ApplySweepClamp(manager, hit->entityA, motionA, hit->normal, hit->time);
               const bool movedB = normalMotionB < -0.0001f && ApplySweepClamp(manager, hit->entityB, motionB, inverseNormal, hit->time);
               if (!movedA && !movedB)
                   continue;

               auto& colliderA = manager.GetComponent<ColliderComp>(hit->entityA, ColliderComp::Name);
               auto& colliderB = manager.GetComponent<ColliderComp>(hit->entityB, ColliderComp::Name);
               AddTouch(colliderA.touchingThisFrame, hit->entityB);
               AddTouch(colliderB.touchingThisFrame, hit->entityA);

               const float invMassA = InverseMass(manager, hit->entityA);
               const float invMassB = InverseMass(manager, hit->entityB);
               ApplyVelocityResponse(manager, hit->entityA, hit->entityB, hit->normal, invMassA, invMassB);
           }
        }

         sf::Vector2f ComputeAABBCorrection(const sf::FloatRect& boxA, const sf::FloatRect& boxB)
        {
            const float moveLeft = boxB.position.x - (boxA.position.x + boxA.size.x);
            const float moveRight = (boxB.position.x + boxB.size.x) - boxA.position.x;
            const float moveUp = boxB.position.y - (boxA.position.y + boxA.size.y);
            const float moveDown = (boxB.position.y + boxB.size.y) - boxA.position.y;

            const float bestX = std::abs(moveLeft) <= std::abs(moveRight) ? moveLeft : moveRight;
            const float bestY = std::abs(moveUp) <= std::abs(moveDown) ? moveUp : moveDown;

            if (std::abs(bestX) <= std::abs(bestY))
                return { bestX, 0.0f };

            return { 0.0f, bestY };
        }

         bool IntersectCircleCircle(const sf::Vector2f& centerA, float radiusA, const sf::Vector2f& centerB, float radiusB, sf::Vector2f& correctionForA)
        {
            const sf::Vector2f delta = centerA - centerB;
            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            const float combinedRadius = radiusA + radiusB;
            const float combinedRadiusSquared = combinedRadius * combinedRadius;
            if (distanceSquared >= combinedRadiusSquared)
                return false;

            const float distance = std::sqrt(distanceSquared);
            if (distance > 0.0001f)
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

         bool IntersectAABBCircle(const sf::FloatRect& boxA, const sf::Vector2f& circleCenter, float circleRadius, sf::Vector2f& correctionForA)
        {
            const float closestX = std::clamp(circleCenter.x, boxA.position.x, boxA.position.x + boxA.size.x);
            const float closestY = std::clamp(circleCenter.y, boxA.position.y, boxA.position.y + boxA.size.y);
            sf::Vector2f delta = { closestX - circleCenter.x, closestY - circleCenter.y };

            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            const float radiusSquared = circleRadius * circleRadius;
            if (distanceSquared >= radiusSquared)
                return false;

            if (distanceSquared > 0.0001f)
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

         bool ComputePairCorrection(EntityManager& manager, UUID entityA, UUID entityB, sf::Vector2f& correctionForA)
        {
            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            auto& colliderArray = manager.GetArray<ColliderComp>(ColliderComp::Name);
            if (!transformArray.m_IdToIndex.count(entityA) || !transformArray.m_IdToIndex.count(entityB))
                return false;
            if (!colliderArray.m_IdToIndex.count(entityA) || !colliderArray.m_IdToIndex.count(entityB))
                return false;

            auto& colliderA = manager.GetComponent<ColliderComp>(entityA, ColliderComp::Name);
            auto& colliderB = manager.GetComponent<ColliderComp>(entityB, ColliderComp::Name);

            auto& transformA = manager.GetComponent<TransformComp>(entityA, TransformComp::Name);
            auto& transformB = manager.GetComponent<TransformComp>(entityB, TransformComp::Name);

            const bool collided = ComputePairCorrection(colliderA, transformA.GetPosition(), colliderB, transformB.GetPosition(), correctionForA);

            if (!collided)
                return false;

            AddTouch(colliderA.touchingThisFrame, entityB);
            AddTouch(colliderB.touchingThisFrame, entityA);
            return true;
        }

         void AddTouch(std::vector<UUID>& touched, UUID entity)
        {
            if (std::find(touched.begin(), touched.end(), entity) == touched.end())
                touched.push_back(entity);
        }

         float InverseMass(EntityManager& manager, UUID entity)
        {
            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            if (!physicsArray.m_IdToIndex.count(entity))
                return 0.0f;

            auto& physics = manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
            if (physics.bodyType != BodyType::Dynamic)
                return 0.0f;
            if (physics.mass <= 0.0001f)
                return 0.0f;
            return 1.0f / physics.mass;
        }

         PhysicsComp* GetPhysicsIfPresent(EntityManager& manager, UUID entity)
        {
            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            if (!physicsArray.m_IdToIndex.count(entity))
                return nullptr;
            return &manager.GetComponent<PhysicsComp>(entity, PhysicsComp::Name);
        }

         const FrameMotion& GetFrameMotion(EntityManager& manager, UUID entity)
        {
            auto cached = m_FrameMotionCache.find(entity);
            if (cached != m_FrameMotionCache.end())
                return cached->second;

            return m_FrameMotionCache.emplace(entity, ComputeFrameMotion(manager, entity, m_FrameMotionDt)).first->second;
        }

         void SyncMovementVelocityIfPresent(EntityManager& manager, UUID entity, const PhysicsComp* physics)
        {
            if (!physics)
                return;

            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            if (!movementArray.m_IdToIndex.count(entity))
                return;

            auto& movement = manager.GetComponent<MovementComp>(entity, MovementComp::Name);
            movement.m_Velocity = physics->velocity;
        }

         void ApplyVelocityResponse(EntityManager& manager, UUID entityA, UUID entityB, const sf::Vector2f& normal, float invMassA, float invMassB)
        {
            const float totalInvMass = invMassA + invMassB;
            if (totalInvMass <= 0.0f)
                return;

            if (IsZeroVector(normal))
                return;
            PhysicsComp* physA = GetPhysicsIfPresent(manager, entityA);
            PhysicsComp* physB = GetPhysicsIfPresent(manager, entityB);

            const sf::Vector2f velocityA = physA ? physA->velocity : sf::Vector2f{ 0.0f, 0.0f };
            const sf::Vector2f velocityB = physB ? physB->velocity : sf::Vector2f{ 0.0f, 0.0f };
            const sf::Vector2f relativeVelocity = velocityA - velocityB;
            const float velAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
            float impulseMagnitude = 0.0f;
            if (velAlongNormal < 0.0f)
            {
                float restitutionA = 0.0f;
                float restitutionB = 0.0f;
                if (physA)
                    restitutionA = PhysicsSystem::ResolveRestitution(*physA);
                if (physB)
                    restitutionB = PhysicsSystem::ResolveRestitution(*physB);
                const float restitution = std::max(restitutionA, restitutionB);

                impulseMagnitude = -(1.0f + restitution) * velAlongNormal / totalInvMass;
                const sf::Vector2f impulse = normal * impulseMagnitude;

                if (physA && invMassA > 0.0f)
                {
                    physA->velocity += impulse * invMassA;
                }
                if (physB && invMassB > 0.0f)
                {
                    physB->velocity -= impulse * invMassB;
                }
            }
            if (impulseMagnitude <= 0.0f)
            {
                SyncMovementVelocityIfPresent(manager, entityA, physA);
                SyncMovementVelocityIfPresent(manager, entityB, physB);
                return;
            }

            const sf::Vector2f postVelocityA = physA ? physA->velocity : sf::Vector2f{ 0.0f, 0.0f };
            const sf::Vector2f postVelocityB = physB ? physB->velocity : sf::Vector2f{ 0.0f, 0.0f };
            const sf::Vector2f postRelativeVelocity = postVelocityA - postVelocityB;

            sf::Vector2f tangent = postRelativeVelocity - normal * (postRelativeVelocity.x * normal.x + postRelativeVelocity.y * normal.y);
            const float tangentLength = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
            if (tangentLength <= 0.0001f)
            {
                SyncMovementVelocityIfPresent(manager, entityA, physA);
                SyncMovementVelocityIfPresent(manager, entityB, physB);
                return;
            }
            tangent /= tangentLength;

            float frictionA = 0.0f;
            float frictionB = 0.0f;
            if (physA)
                frictionA = PhysicsSystem::ResolveFriction(*physA);
            if (physB)
                frictionB = PhysicsSystem::ResolveFriction(*physB);
            const float friction = std::max(frictionA, frictionB);
            if (friction <= 0.0f)
            {
                SyncMovementVelocityIfPresent(manager, entityA, physA);
                SyncMovementVelocityIfPresent(manager, entityB, physB);
                return;
            }

            float frictionImpulseMagnitude = -(postRelativeVelocity.x * tangent.x + postRelativeVelocity.y * tangent.y) / totalInvMass;
            const float maxFrictionImpulse = impulseMagnitude * friction;
            frictionImpulseMagnitude = std::clamp(frictionImpulseMagnitude, -maxFrictionImpulse, maxFrictionImpulse);
            const sf::Vector2f frictionImpulse = tangent * frictionImpulseMagnitude;

            if (physA && invMassA > 0.0f)
            {
                physA->velocity += frictionImpulse * invMassA;
            }
            if (physB && invMassB > 0.0f)
            {
                physB->velocity -= frictionImpulse * invMassB;
            }

            SyncMovementVelocityIfPresent(manager, entityA, physA);
            SyncMovementVelocityIfPresent(manager, entityB, physB);
        }

         void ApplyCorrection(EntityManager& manager, UUID entity, const sf::Vector2f& correction)
        {
            if (IsZeroVector(correction))
                return;

            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            if (transformArray.m_IdToIndex.count(entity))
            {
                auto& transform = manager.GetComponent<TransformComp>(entity, TransformComp::Name);
                transform.Move(correction);
            }

            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            if (movementArray.m_IdToIndex.count(entity))
            {
                auto& movement = manager.GetComponent<MovementComp>(entity, MovementComp::Name);
                movement.m_WasCorrectedByPhysics = true;
            }
        }

         bool ResolvePair(EntityManager& manager, UUID entityA, UUID entityB, const sf::Vector2f& correctionForA)
        {
            if (IsZeroVector(correctionForA))
                return false;

            const float invMassA = InverseMass(manager, entityA);
            const float invMassB = InverseMass(manager, entityB);
            const float totalInvMass = invMassA + invMassB;
            if (totalInvMass <= 0.0f)
                return false;

            if (invMassA > 0.0f)
            {
                const sf::Vector2f correctionA = correctionForA * (invMassA / totalInvMass);
                ApplyCorrection(manager, entityA, correctionA);
            }
            if (invMassB > 0.0f)
            {
                const sf::Vector2f correctionB = { -correctionForA.x * (invMassB / totalInvMass), -correctionForA.y * (invMassB / totalInvMass) };
                ApplyCorrection(manager, entityB, correctionB);
            }

            const float correctionLength = std::sqrt(correctionForA.x * correctionForA.x + correctionForA.y * correctionForA.y);
            if (correctionLength <= 0.0001f)
                return true;

            ApplyVelocityResponse(manager, entityA, entityB, correctionForA / correctionLength, invMassA, invMassB);
            return true;
        }

        Quadtree quadtree;
        CollisionSystemConfig m_Config;
        std::unordered_map<UUID, FrameMotion> m_FrameMotionCache;
        float m_FrameMotionDt = 0.0f;
    };
}
