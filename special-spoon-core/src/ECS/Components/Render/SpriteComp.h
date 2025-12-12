#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Sprite.hpp"

namespace Spoon
{
    struct SpriteComp : public Component
    {
        SpriteComp(sf::Texture& asset, bool centered = false) : m_Sprite(asset) { if (centered) { CenterOrigin(); } }
        SpriteComp(sf::Texture& asset, const sf::IntRect& rect, bool centered = false) : m_Sprite(asset), m_TextureRect(rect) 
        {
            m_Sprite.setTextureRect(rect);
            if (centered) { CenterOrigin(); }
        }
        ~SpriteComp() {}
        
        sf::Sprite m_Sprite;
        sf::IntRect m_TextureRect;

        sf::Vector2f GetPosition() { return m_Sprite.getPosition(); }
        sf::FloatRect GetBoundingBox() { return m_Sprite.getGlobalBounds(); }

        void SetTextureRect(const sf::IntRect& rect) { m_Sprite.setTextureRect(rect); }
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