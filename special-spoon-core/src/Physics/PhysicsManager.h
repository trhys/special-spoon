#pragma once

namespace Spoon
{
    class Scene;
    
    class PhysicsManager
    {
    public:
        PhysicsManager() {}
        ~PhysicsManager() {}

        void CheckCollision(Scene& sceneroot) { m_CD.Detect(sceneroot); }

    private:
        CollisionDetector m_CD;
    };
}