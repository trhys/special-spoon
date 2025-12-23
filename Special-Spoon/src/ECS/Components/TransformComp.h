#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Transformable.hpp"

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

        sf::Transformable m_Transform;
        sf::Vector2f m_CurrentPosition;

        sf::Vector2f iPos;
        sf::Vector2f iScale;
        float iRot;
                
        const sf::Transform& GetTransform() { return m_Transform.getTransform(); }
        sf::Vector2f GetPosition() { return m_Transform.getPosition(); }
        sf::Vector2f GetScale() { return m_Transform.getScale(); }

        void SetPosition(sf::Vector2f pos) { m_Transform.setPosition(pos); }
        void SetScale(sf::Vector2f scale) { m_Transform.setScale(scale); }
        void SetRotation(float rot) { m_Transform.setRotation(sf::degrees(rot)); }
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
                iScale = newScale;
                m_Transform.setScale(newScale);
            }

            float degrees = m_Transform.getRotation().asDegrees();
            ImGui::SeparatorText("Rotation");
            if(ImGui::SliderFloat("##Rotation", &degrees, 0.0f, 360.0f, "%.3f"))
            {
                iRot = degrees;
                m_Transform.setRotation(sf::degrees(degrees));
            }
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TransformComp, iPos, iScale, iRot)
}