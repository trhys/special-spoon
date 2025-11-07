#include "Scene.h"
#include "Entity.h"

namespace Spoon
{

    void Node::draw(sf::RenderTarget& target, sf::Transform& transform)
            {
                sf::Transform combined_transform = transform * m_Transform; 
                
                // For the top of the graph - transform is identity transform -> sf::Transform::Transform()
                // This gives the combined_transform the same value as m_Transform

                OnDraw(target, combined_transform);

                for (auto& child : m_Children)
                {
                    child->draw(target, combined_transform);
                }
            }

    void Scene::CreateEntity()
    {
        m_Children.emplace_back(Entity());
    }

    void Scene::OnDraw(sf::RenderTarget& target, const sf::Transform& transform)
    {
        target.draw(m_Sprite, transform);
    }
}