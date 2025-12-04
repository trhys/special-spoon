#pragma once

#include "Component.h"
#include "SFML/Graphics/Sprite.hpp"

namespace Spoon
{
    struct SpriteComp : public Component
    {
        SpriteComp(sf::Texture& asset) : m_Sprite(asset) {}
        ~SpriteComp() {}
        
        sf::Sprite m_Sprite;

        sf::Vector2f GetPosition() { return m_Sprite.getPosition(); }
        sf::FloatRect GetBoundingBox() { return m_Sprite.getGlobalBounds(); }

        void SetColor(sf::Color color) { m_Sprite.setColor(color); }
        void SetScale(sf::Vector2f scale) { m_Sprite.setScale(scale); }
        void SetPosition(sf::Vector2f pos) { m_Sprite.setPosition(pos); }
        void CenterOrigin() 
        {
            sf::FloatRect bounds = m_Sprite.getLocalBounds(); 
            m_Sprite.setOrigin({ bounds.size.x / 2.0f, bounds.size.y / 2.0f });
        }
    };
}