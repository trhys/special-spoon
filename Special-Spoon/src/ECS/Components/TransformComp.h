#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Transformable.hpp"

namespace Spoon
{
    struct TransformComp : public ComponentBase<TransformComp>
    {
        TransformComp() : ComponentBase::ComponentBase("TransformComp") {}
        TransformComp(sf::Vector2f pos) : ComponentBase::ComponentBase("TransformComp") { SetPosition(pos); }
        ~TransformComp() {}

        sf::Transformable m_Transform;
        sf::Vector2f m_CurrentPosition;
        
        const sf::Transform& GetTransform() { return m_Transform.getTransform(); }
        sf::Vector2f GetPosition() { return m_Transform.getPosition(); }

        void SetPosition(sf::Vector2f pos) { m_Transform.setPosition(pos); }
        void Move(sf::Vector2f offset) { m_Transform.move(offset); }
        void ResetPos() { m_Transform.setPosition(m_CurrentPosition); }
        void SaveCurrentPos() { m_CurrentPosition = GetPosition(); }

        void OnReflect() override
        {
            ImGui::SeparatorText("Position");
            ImGui::Text("X: %s", std::to_string(m_CurrentPosition.x).c_str());
            ImGui::Text("Y: %s", std::to_string(m_CurrentPosition.y).c_str());
        }
    };
}