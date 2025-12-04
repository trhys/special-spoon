#include "CollisionDetector.h"
#include "Scene/Scene.h"

namespace Spoon
{
    // void CollisionDetector::Detect(Scene& sceneroot)
    // {
    //     // Broad phase
    //     auto found = m_BuiltTrees.find(sceneroot.GetName());
    //     if(found == m_BuiltTrees.end())
    //     {
    //         Quadtree quadtree;
    //         quadtree.BuildTree(sceneroot.GetBounds());
    //         m_BuiltTrees.emplace(sceneroot.GetName(), std::move(quadtree));
    //     }
    //     m_ActiveQuadtree = &m_BuiltTrees[sceneroot.GetName()];
    //     m_ActiveQuadtree->Populate(sceneroot);

    //     // Collision check
    //     m_ActiveQuadtree->ProcessCollisionBuffer();
    // }
}