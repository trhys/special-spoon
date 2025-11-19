#pragma once

#include "QuadTree.h"
#include <unordered_map>

namespace Spoon
{
    class Scene;

    class CollisionDetector
    {
    public:
        CollisionDetector() {}
        ~CollisionDetector() {}

        void Detect(Scene& sceneroot);

        Quadtree& GetTree() { return *m_ActiveQuadtree; }

    private:
        Quadtree* m_ActiveQuadtree = nullptr;        
        std::unordered_map<std::string, Quadtree> m_BuiltTrees;
    };
}