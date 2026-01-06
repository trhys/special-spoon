#pragma once

#include "Spoon.h"

struct MovementComp : public Spoon::ComponentBase<MovementComp>
{
    MovementComp(float speed = 0) : ComponentBase::ComponentBase(MovementComp::Name), m_Speed(speed) {}

    static constexpr const char* Name = "Movement";
    float m_Speed;
    sf::Vector2f m_Velocity;
    std::string m_FacingLR; // Left, right
    std::string m_FacingUD; // Up, down

    void OnReflect() override
    {
        if (ImGui::SliderFloat("Speed", &m_Speed, 0.0, 100)) {}
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MovementComp, m_Speed)