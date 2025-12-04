#pragma once

#include "Component.h"
#include "SFML/Graphics/Rect.hpp"

namespace Spoon
{
    struct PhysicsComp : public Component
    {
    public:
        PhysicsComp() {}
        PhysicsComp(sf::FloatRect rect) : m_CollisionBox(rect) {}
        ~PhysicsComp() {}
        
        void SetPosition(sf::Vector2f pos) 
        { 
            m_CollisionBox.position.x = pos.x;
            m_CollisionBox.position.y = pos.y;
        }

        sf::FloatRect GetCollisionBox() { return m_CollisionBox; }

    private:
        sf::FloatRect m_CollisionBox;
    };
}