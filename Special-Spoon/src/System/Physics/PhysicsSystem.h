#pragma once

#include "System/System.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"
#include "System/Physics.PhysicsSystemConfig.h"

#include <algorithm>
#include <cmath>

namespace Spoon
{
    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem() : ISystem::ISystem("Physics") {}
        PhysicsSystem(PhysicsSystemConfig& config) : ISystem::ISystem("Physics") { m_Config = config; }
        ~PhysicsSystem() {}

        static PhysicsSystemConfig& GetConfig() { return m_Config; }
        void SetConfig(PhysicsSystemConfig& c) { m_Config = c; }

        void OnReflect() override
        {
            auto tooltip = [](const char* text)
            {
                if (ImGui::BeginItemTooltip())
                {
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted(text);
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
            };

            ImGui::SeparatorText("Physics Defaults");
            ImGui::SliderFloat("Default Linear Damping##physsystem", &m_Config.defaultLinearDamping, 0.0f, 20.0f, "%.3f");
            tooltip("Fallback for PhysicsComp linear damping when value is -1.");
            ImGui::SliderFloat("Default Friction##physsystem", &m_Config.defaultFriction, 0.0f, 2.0f, "%.2f");
            tooltip("Fallback for PhysicsComp friction when value is -1.");
            ImGui::SliderFloat("Default Restitution##physsystem", &m_Config.defaultRestitution, 0.0f, 1.0f, "%.2f");
            tooltip("Fallback for PhysicsComp restitution when value is -1.");
            ImGui::SliderFloat("Max Linear Speed##physsystem", &m_Config.maxLinearSpeed, 0.0f, 500.0f, "%.2f");
            tooltip("Caps dynamic body speed. 0 disables speed clamping.");
            ImGui::SliderFloat("Sleep Speed Threshold##physsystem", &m_Config.sleepSpeedThreshold, 0.0f, 0.5f, "%.3f");
            tooltip("When sleep snap is enabled, speeds below this threshold are snapped to zero.");
            ImGui::Checkbox("Enable Sleep Snap##physsystem", &m_Config.enableSleepSnap);
            tooltip("Snaps tiny velocities to zero to prevent micro-sliding.");
            ImGui::Checkbox("Clamp Negative Inputs##physsystem", &m_Config.clampNegativeInputs);
            tooltip("Clamps resolved damping/friction/restitution values to non-negative.");
        }

        void Update(sf::Time tick, EntityManager& manager) override
        {
            const float dt = tick.asSeconds();
            if (dt <= 0.0f)
                return;

            auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
            for (size_t index = 0; index < physicsArray.m_Components.size(); index++)
            {
                auto& physicsComp = physicsArray.m_Components[index];
                UUID id = physicsArray.m_IndexToId[index];
                if (!transformArray.m_IdToIndex.count(id))
                    continue;

                if (physicsComp.bodyType == BodyType::Static)
                    continue;

                if (movementArray.m_IdToIndex.count(id))
                {
                    auto& movementComp = manager.GetComponent<MovementComp>(id, MovementComp::Name);
                    if (physicsComp.bodyType == BodyType::Kinematic || movementComp.m_FrameIntent.x != 0.0f)
                    {
                        physicsComp.velocity.x = movementComp.m_Velocity.x;
                    }
                    if (physicsComp.bodyType == BodyType::Kinematic || movementComp.m_FrameIntent.y != 0.0f || physicsComp.gravityScale == 0.0f)
                    {
                        physicsComp.velocity.y = movementComp.m_Velocity.y;
                    }
                }

                if (physicsComp.bodyType == BodyType::Dynamic)
                {
                    physicsComp.velocity.y += k_Gravity * physicsComp.gravityScale * dt;

                    const float damping = ResolveCompValue(physicsComp.linearDamping, m_Config.defaultLinearDamping, m_Config.clampNegativeInputs);
                    const float dampingFactor = std::max(0.0f, 1.0f - damping * dt);
                    physicsComp.velocity *= dampingFactor;

                    if (m_Config.maxLinearSpeed > 0.0f)
                    {
                        const float speedSq = physicsComp.velocity.x * physicsComp.velocity.x + physicsComp.velocity.y * physicsComp.velocity.y;
                        const float maxSpeedSq = m_Config.maxLinearSpeed * m_Config.maxLinearSpeed;
                        if (speedSq > maxSpeedSq)
                        {
                            const float speed = std::sqrt(speedSq);
                            if (speed > 0.0f)
                            {
                                physicsComp.velocity *= (m_Config.maxLinearSpeed / speed);
                            }
                        }
                    }

                    if (m_Config.enableSleepSnap)
                    {
                        const float threshold = std::max(0.0f, m_Config.sleepSpeedThreshold);
                        const float speedSq = physicsComp.velocity.x * physicsComp.velocity.x + physicsComp.velocity.y * physicsComp.velocity.y;
                        if (speedSq < threshold * threshold)
                        {
                            physicsComp.velocity = { 0.0f, 0.0f };
                        }
                    }
                }

                const sf::Vector2f delta = physicsComp.velocity * dt;
                if (delta.x == 0.0f && delta.y == 0.0f)
                {
                    if (movementArray.m_IdToIndex.count(id))
                    {
                        auto& movementComp = manager.GetComponent<MovementComp>(id, MovementComp::Name);
                        movementComp.m_ProposedDelta = delta;
                        movementComp.m_Velocity = physicsComp.velocity;
                    }
                    continue;
                }

                TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                transform.Move(delta);

                if (movementArray.m_IdToIndex.count(id))
                {
                    auto& movementComp = manager.GetComponent<MovementComp>(id, MovementComp::Name);
                    movementComp.m_ProposedDelta = delta;
                    movementComp.m_Velocity = physicsComp.velocity;
                }
            }
        }

        static float ResolveFriction(const PhysicsComp& comp)
        {
            return ResolveCompValue(comp.friction, m_Config.defaultFriction, m_Config.clampNegativeInputs);
        }

        static float ResolveRestitution(const PhysicsComp& comp)
        {
            return ResolveCompValue(comp.restitution, m_Config.defaultRestitution, m_Config.clampNegativeInputs);
        }

        static float ResolveLinearDamping(const PhysicsComp& comp)
        {
            return ResolveCompValue(comp.linearDamping, m_Config.defaultLinearDamping, m_Config.clampNegativeInputs);
        }

    private:
        float ResolveCompValue(float componentValue, float defaultValue, bool clampNegativeInputs)
        {
            float resolved = componentValue >= 0.0f ? componentValue : defaultValue;
            if (clampNegativeInputs && resolved < 0.0f)
                resolved = 0.0f;
            return resolved;
        }

    private:
        static constexpr float k_Gravity = 980.0f;
        static PhysicsSystemConfig m_Config;
    };
}
