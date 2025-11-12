#include "CollisionDetector.h"
#include "Scene/Scene.h"
#include <optional>

namespace Spoon
{
    void CollisionDetector::BuildQT()
    {
        // TODO - BUILD QUADTREE
    }
    void CollisionDetector::Detect(const Scene& sceneroot)
    {
        // TODO - BROAD PHASE

        // Check for bounding box intersection
        for(auto a = 0; a < sceneroot.GetChildren().size(); a++)
        {
            for(auto b = a + 1; b < sceneroot.GetChildren().size(); b++)
            {
                if (const std::optional collision = sceneroot.GetChildren()[a]->GetBoundingBox().findIntersection(sceneroot.GetChildren()[b]->GetBoundingBox()))
                {
                    // Handle collision
                    std::cout << "COLLISION DETECTED" << std::endl;
                }
            }
        }
    }
}