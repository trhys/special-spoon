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
            ImGui::Text("POS X: %.2f", m_CollisionBox.position.x);
            ImGui::Text("POS Y: %.2f", m_CollisionBox.position.y);
            ImGui::Text("Collision detected? : %s", Collided ? "True" : "False");
        }
        
        sf::FloatRect m_CollisionBox;
        bool Collided = false;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PhysicsComp, Collided)
}