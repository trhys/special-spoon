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
        sf::Vector2f GetScale() { return m_Transform.getScale(); }

        void SetPosition(sf::Vector2f pos) { m_Transform.setPosition(pos); }
        void Move(sf::Vector2f offset) { m_Transform.move(offset); }
        void ResetPos() { m_Transform.setPosition(m_CurrentPosition); }
        void SaveCurrentPos() { m_CurrentPosition = GetPosition(); }

        void OnReflect() override
        {
            ImGui::SeparatorText("Position");
            ImGui::Text("X: %s", std::to_string(m_Transform.getPosition().x).c_str());
            ImGui::Text("Y: %s", std::to_string(m_Transform.getPosition().y).c_str());

            float tranScale[2] = { m_Transform.getScale().x, m_Transform.getScale().y };
            ImGui::SeparatorText("Scale");
            if(ImGui::SliderFloat2("##Scale", tranScale, -10.0f, 10.0f, "%.3f"))
            {
                sf::Vector2f newScale = { tranScale[0], tranScale[1] };
                m_Transform.setScale(newScale);
            }

            float degrees = m_Transform.getRotation().asDegrees();
            ImGui::SeparatorText("Rotation");
            if(ImGui::SliderFloat("##Rotation", &degrees, 0.0f, 360.0f, "%.3f"))
            {
                m_Transform.setRotation(sf::degrees(degrees));
            }
        }
    };
}