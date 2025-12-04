#pragma once

#include "Component.h"
#include "SFML/Graphics/Transformable.hpp"

namespace Spoon
{
    struct TransformComp : public Component
    {
        TransformComp() {}
        TransformComp(sf::Vector2f pos) { SetPosition(pos); }
        ~TransformComp() {}

        sf::Transformable m_Transform;
        sf::Vector2f m_CurrentPosition;
        
        const sf::Transform& GetTransform() { return m_Transform.getTransform(); }
        sf::Vector2f GetPosition() { return m_Transform.getPosition(); }

        void SetPosition(sf::Vector2f pos) { m_Transform.setPosition(pos); }
        void Move(sf::Vector2f offset) { m_Transform.move(offset); }
        void ResetPos() { m_Transform.setPosition(m_CurrentPosition); }
        void SaveCurrentPos() { m_CurrentPosition = GetPosition(); }
    };
}