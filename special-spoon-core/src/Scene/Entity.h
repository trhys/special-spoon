#pragma once

#include "Node.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public Node
    {
    public:
        Entity(sf::Texture& texture) : m_Sprite(texture) {}
        Entity(sf::Texture& texture, bool collidable) : m_Sprite(texture), Node::Node(collidable) {}
        virtual ~Entity() {}

        virtual void OnAdd() {}

        void ScaleSprite(sf::Vector2f scale) { m_Sprite.setScale(scale); }
        void SetSpritePosition(sf::Vector2f pos) { m_Sprite.setPosition(pos); }
        sf::Vector2f GetSpritePosition() { return m_Sprite.getPosition(); }
        sf::FloatRect GetBoundingBox() override { return m_Sprite.getGlobalBounds(); }

        void CenterOrigin() 
        {
            sf::FloatRect bounds = m_Sprite.getLocalBounds(); 
            m_Sprite.setOrigin({ bounds.size.x / 2.0f, bounds.size.y / 2.0f });
        }

    private:
        sf::Sprite m_Sprite;

        void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(m_Sprite, states);
        }
    };
}