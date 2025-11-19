#pragma once

#include "CollisionDetector.h"

namespace Spoon
{
    class Scene;
    struct QT_GridNode;

    class PhysicsManager
    {
    public:
        PhysicsManager() {}
        ~PhysicsManager() {}

        void CheckCollision(Scene& sceneroot) { m_CD.Detect(sceneroot); }   
        //std::vector<QT_GridNode> PhysTest() { return m_CD.GetTree().GetNodes(); }

    private:
        CollisionDetector m_CD;
    };
}