#pragma once

#include "ECS/Components/Component.h"

#include <cmath>

namespace Spoon {
 
  struct FacingDirection
  {
      float m_Angle = 0.0f; // In degrees
      const char* m_Direction = "Down";

      void UpdateDirection()
      {
          // Normalize angle to [0, 360)
          float normalizedAngle = std::fmod(m_Angle + 360.f, 360.f);

          // Map to 8 directions: each occupies 45 degrees, centered on cardinal/diagonal
          if (normalizedAngle >= 337.5f || normalizedAngle < 22.5f)
              m_Direction = "Right";
          else if (normalizedAngle < 67.5f)
              m_Direction = "DownRight";
          else if (normalizedAngle < 112.5f)
              m_Direction = "Down";
          else if (normalizedAngle < 157.5f)
              m_Direction = "DownLeft";
          else if (normalizedAngle < 202.5f)
              m_Direction = "Left";
          else if (normalizedAngle < 247.5f)
              m_Direction = "UpLeft";
          else if (normalizedAngle < 292.5f)
              m_Direction = "Up";
          else
              m_Direction = "UpRight";
      }
  };

  struct MovementComp : public ComponentBase<MovementComp>
  {
      MovementComp(float speed = 0) : ComponentBase::ComponentBase(MovementComp::Name), m_Speed(speed) {}

      static constexpr const char* Name = "Movement";

      // Core members
      float m_Speed;

      // Runtime members
      sf::Vector2f m_Velocity;
      FacingDirection m_Facing;
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
          ImGui::Text("Facing: %s", m_Facing.m_Direction);
          ImGui::Text("Corrected By Physics: %s", m_WasCorrectedByPhysics ? "True" : "False");
      }
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MovementComp, m_Speed)
}
