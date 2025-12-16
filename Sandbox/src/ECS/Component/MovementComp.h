#pragma once

#include "Spoon.h"

struct MovementComp : public Spoon::Component
{
    MovementComp(float speed) : m_Speed(speed) {}

    float m_Speed;
    sf::Vector2f m_Velocity;
    std::string m_FacingLR; // Left, right
    std::string m_FacingUD; // Up, down
};