#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

namespace Spoon
{
    struct TransformComp : public ComponentBase<TransformComp>
    {
        TransformComp() : ComponentBase::ComponentBase("Transform") {}
        TransformComp(sf::Vector2f pos, sf::Vector2f scale, float rotation) : ComponentBase::ComponentBase("Transform"), iPos(pos), iScale(scale), iRot(rotation)
        {
            SetPosition(pos);
            SetScale(scale);
            SetRotation(rotation);
        }
        ~TransformComp() {}

        // Core members
        sf::Transformable m_Transform;
        sf::Vector2f m_CurrentPosition;

        // Serialized members
        sf::Vector2f iPos;
        sf::Vector2f iScale;
        float iRot = 0.0;

        // Reflection members for editor
        sf::RectangleShape rect;
        sf::Vector2f drag;
        bool dragging = false;
                
        // Core getters
        const sf::Transform& GetTransform() { return m_Transform.getTransform(); }
        sf::Vector2f GetPosition() { return m_Transform.getPosition(); }
        sf::Vector2f GetScale() { return m_Transform.getScale(); }

        // Core setters
        void SetPosition(sf::Vector2f pos) { m_Transform.setPosition(pos); }
        void SetScale(sf::Vector2f scale) { m_Transform.setScale(scale); }
        void SetRotation(float rot) { m_Transform.setRotation(sf::degrees(rot)); }
        void Move(sf::Vector2f offset) { m_Transform.move(offset); }
        void ResetPos() { m_Transform.setPosition(m_CurrentPosition); }
        void SaveCurrentPos() { m_CurrentPosition = GetPosition(); }

        // Editor methods
        void OnReflect() override;
        void MoveTransform();
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TransformComp, iPos, iScale, iRot)
}