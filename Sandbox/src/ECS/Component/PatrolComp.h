#pragma once

#include "Spoon.h"

struct PatrolComp : public Spoon::ComponentBase<PatrolComp>
{    
    PatrolComp(std::vector<sf::Vector2f> patrolPoints = {}, float idleTime = 0.0f) 
     : ComponentBase::ComponentBase("PatrolComp"), m_PatrolPoints(patrolPoints), m_IdleTime(idleTime) {}

    float m_IdleTime;
    float m_CurrentIdle = 0.0f;

    std::vector<sf::Vector2f> m_PatrolPoints;
    sf::Vector2f m_Destination;
    int m_CurrentPointIndex = 0;

    // Reflection member for editor
    sf::Vector2f selectedPoint;

    void NextPoint()
    {
        if (m_PatrolPoints.size() == 0) return;
        else if (m_CurrentPointIndex >= m_PatrolPoints.size() - 1)
            m_CurrentPointIndex = 0;
        else
            m_CurrentPointIndex++;
        m_Destination = m_PatrolPoints[m_CurrentPointIndex];
        m_CurrentIdle = 0.0f;
    }

    void OnReflect() override
    {
        ImGui::Text("Patrol Points: %ld", m_PatrolPoints.size());
        ImGui::Text("Current Destination: (%.2f, %.2f)", m_Destination.x, m_Destination.y);
        ImGui::SeparatorText("Idle Time");
        if (ImGui::DragFloat("##IdleTime", &m_IdleTime, 0.1f, 0.0f, 100.0f))
        {
            if (m_IdleTime < 0.0f) m_IdleTime = 0.0f;
        }

        ImGui::SeparatorText("Patrol Points List");
        if (ImGui::BeginListBox("##PatrolPoints"))
        {
            for (auto& point : m_PatrolPoints)
            {
                // ImGui::Text("(%.2f, %.2f)", point.x, point.y);
                if (ImGui::Selectable((std::string("(") + std::to_string(point.x) + ", " + std::to_string(point.y) + ")").c_str(), selectedPoint == point))
                {
                    selectedPoint = point;
                }
            }
            ImGui::EndListBox();
        }
        if (ImGui::Button(ActiveGizmo() ? "Confirm" : "Add Patrol Point"))
        {
            ToggleGizmo();
        }

        if (ActiveGizmo())
        {
            AddPatrolPoint();
        }
    }

    void AddPatrolPoint()
    {
        auto& viewport = Spoon::Application::Get().GetViewport();
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            ImVec2 viewportPos = viewport.state.viewportPos;
            sf::Vector2i mousePos = sf::Mouse::getPosition(Spoon::Application::Get().GetWindow());
            sf::Vector2f relativePos{
                static_cast<float>(mousePos.x) - viewportPos.x,
                static_cast<float>(mousePos.y) - viewportPos.y
            };
            sf::Vector2f worldPos = viewport.target.mapPixelToCoords(sf::Vector2i(relativePos));
            m_PatrolPoints.push_back(worldPos);
        }
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PatrolComp, m_PatrolPoints, m_IdleTime)