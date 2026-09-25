#pragma once

#include "SFML/Graphics.hpp"

namespace Spoon {
  struct PhysicsSystemConfig
    {
        float defaultLinearDamping = 0.0f;
        float defaultFriction = 0.6f;
        float defaultRestitution = 0.6f;
        float maxLinearSpeed = 0.0f;
        float sleepSpeedThreshold = 0.01f;
        bool gravityEnabled = true;
        bool enableSleepSnap = true;
        bool clampNegativeInputs = true;
    };

  struct CollisionSystemConfig
  {
    sf::Vector2f bounds = {4000.0f, 4000.0f};
  };
}
