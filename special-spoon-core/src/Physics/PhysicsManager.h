#pragma once

#include "CollisionDetector.h"

namespace Spoon
{
    class Scene;
    
    class PhysicsManager
    {
    public:
        PhysicsManager() {}
        ~PhysicsManager() {}

        void CheckCollision(const Scene& sceneroot) { m_CD.Detect(sceneroot); }

    private:
        CollisionDetector m_CD;
    };
}