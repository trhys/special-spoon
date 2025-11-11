#include "CollisionDetector.h"
#include "Scene/Scene.h"

namespace Spoon
{
    void CollisionDetector::BuildQT()
    {
        // TODO - BUILD QUADTREE
    }
    void CollisionDetector::Detect(Scene& sceneroot)
    {
        // TODO - BROAD PHASE

        // Check for bounding box intersection
        for(auto a = 0; a < sceneroot->GetChildren().size(); a++)
        {
            for(auto b = a + 1; b < sceneroot->GetChildren().size(); b++)
            {
                if(const std::optional collision = sceneroot->GetChildren()[a]->getGlobalBounds().findIntersection(sceneroot->GetChildren()[b]->getGlobalBounds()))
                {
                    // Handle collision
                }
            }
        }
    }
}