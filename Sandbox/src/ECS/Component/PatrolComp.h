#include "Spoon.h"

struct PatrolComp : public Spoon::Component
{
public:
    PatrolComp(float pointA, float pointB, float idleTime = 0.0f) : m_PointA(pointA), m_PointB(pointB), m_IdleTime(idleTime) {}

    float m_PointA;
    float m_PointB;
    float m_IdleTime;
    float m_CurrentIdle = 0.0f;
    bool m_MovingToA = false;
    bool m_MovingToB = true;
};