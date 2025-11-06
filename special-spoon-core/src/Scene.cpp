#include "Scene.h"
#include "Entity.h"

namespace Spoon
{

    void Scene::CreateEntity()
    {

    }
    
    void Scene::draw(sf::RenderTarget& target, sf::Transform& scene_transform) const
            {
                sf::Transform combined_transform = scene_transform * m_Transform; 
                
                // For the top of the graph - scene_transform is identity transform -> sf::Transform::Transform()
                // This gives the combined_transform the same value as m_Transform

                OnDraw(target, combined_transform);

                for (auto& child : m_Children)
                {
                    child->draw(target, combined_transform);
                }
            }
}