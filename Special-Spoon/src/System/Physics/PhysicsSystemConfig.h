#pragma once

namespace Spoon {
  struct PhysicsSystemConfig
    {
        float defaultLinearDamping = 0.0f;
        float defaultFriction = 0.6f;
        float defaultRestitution = 0.6f;
        float maxLinearSpeed = 0.0f;
        float sleepSpeedThreshold = 0.01f;
        bool enableSleepSnap = true;
        bool clampNegativeInputs = true;
    };
}
