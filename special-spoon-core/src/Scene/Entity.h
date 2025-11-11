#pragma once

#include "Node.h"
#include "SFML/Graphics.hpp"


namespace Spoon
{
    class Entity : public Node
    {
    public:
        Entity(sf::Texture& texture) : m_Sprite(texture) {}
        virtual ~Entity() {}

        virtual void OnAdd() {}
        virtual void OnKill() {}
        
    private:
        sf::Sprite m_Sprite;

        void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(m_Sprite, states);
        }

        //virtual void OnUpdate(sf::Time tick, Layer* context) override {}
    };
}