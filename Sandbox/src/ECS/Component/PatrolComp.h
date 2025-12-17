#pragma once

#include "Spoon.h"

struct PatrolComp : public Spoon::ComponentBase<PatrolComp>
{
    PatrolComp(sf::Vector2f pointA, sf::Vector2f pointB, float idleTime = 0.0f) : ComponentBase::ComponentBase("PatrolComp"), m_PointA(pointA), m_PointB(pointB), m_IdleTime(idleTime) {}

    sf::Vector2f m_PointA;
    sf::Vector2f m_PointB;
    float m_IdleTime;
    float m_CurrentIdle = 0.0f;
    bool m_MovingToA = false;
    bool m_MovingToB = true;

    void TurnAround()
    {
        m_MovingToA = !m_MovingToA;
        m_MovingToB = !m_MovingToB;
        m_CurrentIdle = 0.0f;
    }
};