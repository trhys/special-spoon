#pragma once

#include <vector>
#include "SFML/Graphics.hpp"

namespace Spoon 
{    
    class Node : public sf::Drawable, public sf::Transformable
    {
    public:
        Node() {}
        virtual ~Node() {}

        void draw(sf::RenderTarget& target, const sf::Transform& transform) const
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

    private:
        virtual void OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const = 0;

        sf::Transform m_Transform;
        std::vector<Node*> m_Children;
    };
}