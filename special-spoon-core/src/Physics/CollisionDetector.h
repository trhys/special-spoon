#pragma once

#include "QuadTree.h"

namespace Spoon
{
    class Scene;

    class CollisionDetector
    {
    public:
        CollisionDetector() {}
        ~CollisionDetector() {}

        void BuildQT();
        void Detect(const Scene& sceneroot);

    private:
        Quadtree m_Quadtree;

    };
}