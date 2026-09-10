#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Rect.hpp"

namespace Spoon
{
    struct PhysicsComp : public ComponentBase<PhysicsComp>
    {
    public:
        PhysicsComp() : ComponentBase::ComponentBase(Name) {}
        PhysicsComp(sf::FloatRect rect) : ComponentBase::ComponentBase(Name), m_CollisionBox(rect) {}

        static constexpr const char* Name = "Physics";

        void SetPosition(sf::Vector2f pos)
        {
            m_CollisionBox.position.x = pos.x;
            m_CollisionBox.position.y = pos.y;
        }

        sf::FloatRect GetCollisionBox() { return m_CollisionBox; }

        void CollisionDetected()
        {
            Collided = true;
        }

        void CollisionHandled()
        {
            Collided = false;
            m_BlockedX = false;
            m_BlockedY = false;
            m_LastCorrection = {0.0f, 0.0f};
        }

        void OnReflect() override
        {
            ImGui::Text("Position: (%.2f, %.2f)", m_CollisionBox.position.x, m_CollisionBox.position.y);
            ImGui::Text("Mass: %.2f", mass);
            ImGui::SliderFloat("Restitution##physics", &restitution, 0.0f, 1.0f, "%.2f");
            ImGui::Checkbox("Is Static##physics", &isStatic);
            ImGui::Text("Collision detected: %s", Collided ? "True" : "False");
            ImGui::Text("Blocked X: %s", m_BlockedX ? "True" : "False");
            ImGui::Text("Blocked Y: %s", m_BlockedY ? "True" : "False");
            ImGui::Text("Correction: (%.2f, %.2f)", m_LastCorrection.x, m_LastCorrection.y);
        }

        sf::FloatRect m_CollisionBox;
        float mass = 1.0f;
        float restitution = 0.6f;
        bool isStatic = false;
        bool Collided = false;
        bool m_BlockedX = false;
        bool m_BlockedY = false;
        sf::Vector2f m_LastCorrection = {0.0f, 0.0f};
    };

    inline void to_json(json& j, const PhysicsComp& comp)
    {
        j = json{
            {"m_CollisionBox", comp.m_CollisionBox},
            {"mass", comp.mass},
            {"restitution", comp.restitution},
            {"isStatic", comp.isStatic},
            {"Collided", comp.Collided}
        };
    }

    inline void from_json(const json& j, PhysicsComp& comp)
    {
        if (j.contains("m_CollisionBox"))
            comp.m_CollisionBox = j.at("m_CollisionBox").get<sf::FloatRect>();
        if (j.contains("mass"))
            comp.mass = j.at("mass").get<float>();
        if (j.contains("restitution"))
            comp.restitution = j.at("restitution").get<float>();
        if (j.contains("isStatic"))
            comp.isStatic = j.at("isStatic").get<bool>();
        if (j.contains("Collided"))
            comp.Collided = j.at("Collided").get<bool>();
    }
}
