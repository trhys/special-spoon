#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
    enum class BodyType
    {
        Static = 0,
        Dynamic = 1,
        Kinematic = 2
    };

    struct PhysicsComp : public ComponentBase<PhysicsComp>
    {
    public:
        PhysicsComp() : ComponentBase::ComponentBase(Name) {}

        static constexpr const char* Name = "Physics";

        static const char* BodyTypeToString(BodyType type)
        {
            switch (type)
            {
            case BodyType::Static:
                return "Static";
            case BodyType::Kinematic:
                return "Kinematic";
            case BodyType::Dynamic:
            default:
                return "Dynamic";
            }
        }

        static BodyType BodyTypeFromString(const std::string& value)
        {
            if (value == "Static")
                return BodyType::Static;
            if (value == "Kinematic")
                return BodyType::Kinematic;
            return BodyType::Dynamic;
        }

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

            const char* bodyTypes[] = { "Static", "Dynamic", "Kinematic" };
            int bodyTypeIndex = static_cast<int>(bodyType);
            if (ImGui::Combo("Body Type##physics", &bodyTypeIndex, bodyTypes, 3))
            {
                bodyType = static_cast<BodyType>(bodyTypeIndex);
            }

            ImGui::SliderFloat("Velocity X##physics", &velocity.x, -2000.0f, 2000.0f, "%.2f");
            tooltip("Horizontal velocity in units per second.");
            ImGui::SliderFloat("Velocity Y##physics", &velocity.y, -2000.0f, 2000.0f, "%.2f");
            tooltip("Vertical velocity in units per second.");
            ImGui::SliderFloat("Mass##physics", &mass, 0.001f, 1000.0f, "%.3f");
            tooltip("Dynamic mass used for collision impulse response.");
            ImGui::SliderFloat("Gravity Scale##physics", &gravityScale, -10.0f, 10.0f, "%.2f");
            tooltip("Multiplier applied to world gravity.");
            ImGui::SliderFloat("Restitution##physics", &restitution, -1.0f, 1.0f, "%.2f");
            tooltip("Bounciness. -1 = use PhysicsSystem default.");
            ImGui::SliderFloat("Friction##physics", &friction, -1.0f, 2.0f, "%.2f");
            tooltip("Contact friction. -1 = use PhysicsSystem default.");
            ImGui::SliderFloat("Linear Damping##physics", &linearDamping, -1.0f, 20.0f, "%.3f");
            tooltip("Velocity damping. -1 = use PhysicsSystem default.");
        }

        BodyType bodyType = BodyType::Dynamic;
        sf::Vector2f velocity = { 0.0f, 0.0f };
        float mass = 1.0f;
        float gravityScale = 1.0f;
        float restitution = -1.0f;
        float friction = -1.0f;
        float linearDamping = -1.0f;
        // Runtime-only sweep bookkeeping; reset by PhysicsSystem each frame.
        sf::Vector2f frameAppliedDelta = { 0.0f, 0.0f };
        // Runtime-only sweep bookkeeping; true when frameAppliedDelta was produced this frame.
        bool frameDeltaComputedThisFrame = false;
        // Runtime-only sweep bookkeeping; true when PhysicsSystem moved TransformComp this frame.
        bool transformAdvancedThisFrame = false;
    };

    inline void to_json(json& j, const PhysicsComp& comp)
    {
        j = json{
            {"bodyType", PhysicsComp::BodyTypeToString(comp.bodyType)},
            {"velocity", comp.velocity},
            {"mass", comp.mass},
            {"gravityScale", comp.gravityScale},
            {"restitution", comp.restitution},
            {"friction", comp.friction},
            {"linearDamping", comp.linearDamping}
        };
    }

    inline void from_json(const json& j, PhysicsComp& comp)
    {
        if (j.contains("bodyType"))
            comp.bodyType = PhysicsComp::BodyTypeFromString(j.at("bodyType").get<std::string>());

        if (j.contains("velocity"))
            comp.velocity = j.at("velocity").get<sf::Vector2f>();
        if (j.contains("mass"))
            comp.mass = j.at("mass").get<float>();
        if (j.contains("gravityScale"))
            comp.gravityScale = j.at("gravityScale").get<float>();
        if (j.contains("restitution"))
            comp.restitution = j.at("restitution").get<float>();
        if (j.contains("friction"))
            comp.friction = j.at("friction").get<float>();
        if (j.contains("linearDamping"))
            comp.linearDamping = j.at("linearDamping").get<float>();
    }
}
