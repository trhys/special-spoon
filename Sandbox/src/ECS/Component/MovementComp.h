#pragma once

#include "Spoon.h"

struct FacingDirection
{
    float m_Angle = 0.0f; // In degrees
    const char* m_Direction = "Right"; // "Up", "Down", "Left", "Right"

    void UpdateDirection()
    {
        if (m_Angle >= 45.f && m_Angle < 135.f)
            m_Direction = "Down";
        else if (m_Angle >= 135.f && m_Angle < 225.f)
            m_Direction = "Left";
        else if (m_Angle >= 225.f && m_Angle < 315.f)
            m_Direction = "Up";
        else
            m_Direction = "Right";
    }
};

struct MovementComp : public Spoon::ComponentBase<MovementComp>
{
    MovementComp(float speed = 0) : ComponentBase::ComponentBase(MovementComp::Name), m_Speed(speed) {}

    static constexpr const char* Name = "Movement";

    // Core members
    float m_Speed;

    // Runtime members
    sf::Vector2f m_Velocity;
    FacingDirection m_Facing;

    void OnReflect() override
    {
        if (ImGui::SliderFloat("Speed", &m_Speed, 0.0, 100)) {}
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MovementComp, m_Speed)