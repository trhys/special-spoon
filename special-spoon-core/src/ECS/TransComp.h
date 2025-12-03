#pragma once

#include "Component.h"
#include "SFML/Graphics/Transformable.hpp"

namespace Spoon
{
    class TransComp : public Component
    {
    public:
        TransComp() {}
        TransComp(sf::Vector2f pos) { SetPosition(pos); }
        ~TransComp() {}

        void Update(sf::Time tick) override {}

        const sf::Transform& GetTransform() { return m_Transform.getTransform(); }
        sf::Vector2f GetPosition() { return m_Transform.getPosition(); }

        void SetPosition(sf::Vector2f pos) { m_Transform.setPosition(pos); }
        void Move(sf::Vector2f offset) { m_Transform.move(offset); }
        void ResetPos() { m_Transform.setPosition(m_CurrentPosition); }
        void SavePos() { m_CurrentPosition = GetPosition(); }

    private:
        sf::Transformable m_Transform;
        sf::Vector2f m_CurrentPosition;
    };
}