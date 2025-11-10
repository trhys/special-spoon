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

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            states.transform *= getTransform();

            OnDraw(target, states);

            for (auto& child : m_Children)
            {
                child->draw(target, states);
            }
        }

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;

        sf::Transform m_Transform;
        std::vector<Node*> m_Children;
    };
}