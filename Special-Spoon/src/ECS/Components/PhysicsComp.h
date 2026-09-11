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
            const char* bodyTypes[] = { "Static", "Dynamic", "Kinematic" };
            int bodyTypeIndex = static_cast<int>(bodyType);
            if (ImGui::Combo("Body Type##physics", &bodyTypeIndex, bodyTypes, 3))
            {
                bodyType = static_cast<BodyType>(bodyTypeIndex);
            }

            ImGui::SliderFloat("Velocity X##physics", &velocity.x, -2000.0f, 2000.0f, "%.2f");
            ImGui::SliderFloat("Velocity Y##physics", &velocity.y, -2000.0f, 2000.0f, "%.2f");
            ImGui::SliderFloat("Mass##physics", &mass, 0.001f, 1000.0f, "%.3f");
            ImGui::SliderFloat("Gravity Scale##physics", &gravityScale, -10.0f, 10.0f, "%.2f");
            ImGui::SliderFloat("Restitution##physics", &restitution, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("Linear Damping##physics", &linearDamping, 0.0f, 20.0f, "%.3f");
        }

        BodyType bodyType = BodyType::Dynamic;
        sf::Vector2f velocity = { 0.0f, 0.0f };
        float mass = 1.0f;
        float gravityScale = 1.0f;
        float restitution = 0.6f;
        float linearDamping = 0.0f;
    };

    inline void to_json(json& j, const PhysicsComp& comp)
    {
        j = json{
            {"bodyType", PhysicsComp::BodyTypeToString(comp.bodyType)},
            {"velocity", comp.velocity},
            {"mass", comp.mass},
            {"gravityScale", comp.gravityScale},
            {"restitution", comp.restitution},
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
        if (j.contains("linearDamping"))
            comp.linearDamping = j.at("linearDamping").get<float>();
    }
}
