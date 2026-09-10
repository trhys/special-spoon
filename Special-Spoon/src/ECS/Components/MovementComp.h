#pragma once

#include "ECS/Components/Component.h"

#include <cmath>

namespace Spoon {
 
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
      bool m_MoveLeftHeld = false;
      bool m_MoveRightHeld = false;
      bool m_MoveUpHeld = false;
      bool m_MoveDownHeld = false;
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
