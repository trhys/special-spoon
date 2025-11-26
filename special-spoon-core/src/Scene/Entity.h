#pragma once

#include "Node.h"
#include "SFML/Graphics.hpp"


namespace Spoon
{
    class Entity : public Node
    {
    public:
        Entity(sf::Texture& texture, bool collidable = false) : m_Sprite(texture), m_IsCollidable(collidable) {}
        virtual ~Entity() {}

        virtual void OnAdd() {}

        void ScaleSprite(sf::Vector2f scale) { m_Sprite.setScale(scale); }
        
        void CenterOrigin() 
        {
            sf::FloatRect bounds = m_Sprite.getLocalBounds(); 
            m_Sprite.setOrigin({ bounds.size.x / 2.0f, bounds.size.y / 2.0f });
        }

        bool GetIsCollidable() override { return m_IsCollidable; }
        sf::FloatRect GetBoundingBox() override { return m_Sprite.getGlobalBounds(); }
        
    private:
        sf::Sprite m_Sprite;
        bool m_IsCollidable;

        void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(m_Sprite, states);
        }
    };
}