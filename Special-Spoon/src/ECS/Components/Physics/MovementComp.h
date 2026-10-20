#pragma once

#include "ECS/Components/Component.h"

#include <cmath>

namespace Spoon 
{
  struct MovementComp : public ComponentBase<MovementComp>
  {
      MovementComp(float speed = 0) : ComponentBase::ComponentBase(MovementComp::Name), m_Speed(speed) {}

      static constexpr const char* Name = "Movement";

      // Core members
      float m_Speed;

      // Runtime members
      sf::Vector2f m_Velocity;
      sf::Vector2f m_FrameIntent;
      sf::Vector2f m_ProposedDelta;
      bool m_WasCorrectedByPhysics = false;

      void OnReflect() override
      {
          if (ImGui::SliderFloat("Speed", &m_Speed, 0.0, 100)) {}
          ImGui::SeparatorText("Runtime");
          ImGui::Text("Intent: (%.2f, %.2f)", m_FrameIntent.x, m_FrameIntent.y);
          ImGui::Text("Velocity: (%.2f, %.2f)", m_Velocity.x, m_Velocity.y);
          ImGui::Text("Proposed Delta: (%.2f, %.2f)", m_ProposedDelta.x, m_ProposedDelta.y);
          ImGui::Text("Corrected By Physics: %s", m_WasCorrectedByPhysics ? "True" : "False");
      }
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MovementComp, m_Speed)
}
