#pragma once

#include "Node.h"
#include "SFML/Graphics.hpp"


namespace Spoon
{
    class Entity : public Node
    {
    public:
        Entity(sf::Texture& texture) : m_Sprite(texture) { m_Sprite.setOrigin(m_Sprite.getLocalBounds().width / 2.0, 0); }
        virtual ~Entity() {}

        virtual void OnAdd() {}
        virtual void OnKill() {}

        void ScaleSprite(sf::Vector2f scale) { m_Sprite.setScale(scale); }

        sf::FloatRect GetBoundingBox() override { return m_Sprite.getGlobalBounds(); }
        
    private:
        sf::Sprite m_Sprite;

        void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(m_Sprite, states);
        }
    };
}