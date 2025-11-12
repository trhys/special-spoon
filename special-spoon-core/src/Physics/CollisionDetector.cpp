#include "Core.h"
#include "CollisionDetector.h"

namespace Spoon
{
    void CollisionDetector::Detect(const Scene& sceneroot)
    {
        // Broad phase
        if(!tree_IsBuilt)
        {
            m_Quadtree.BuildTree(sceneroot.GetBounds());
            tree_IsBuilt = true;
        }
        m_Quadtree.GetCollisionBodies(sceneroot);

        // Collision check
        m_Quadtree.ProcessCollisionBuffer();
    }
}