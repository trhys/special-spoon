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
        }

        void OnReflect() override
        {
            ImGui::Text("Position: (%.2f, %.2f)", m_CollisionBox.position.x, m_CollisionBox.position.y);
            ImGui::Text("Mass: %.2f", mass);
            ImGui::SliderFloat("Restitution##physics", &restitution, 0.0f, 1.0f, "%.2f");
            ImGui::Checkbox("Is Static##physics", &isStatic);
            ImGui::Text("Collision detected: %s", Collided ? "True" : "False");
        }
        
        sf::FloatRect m_CollisionBox;
        float mass = 1.0f;
        float restitution = 0.6f;
        bool isStatic = false;
        bool Collided = false;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PhysicsComp, Collided, mass, restitution, isStatic)
}
