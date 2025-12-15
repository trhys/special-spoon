#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Text.hpp"

namespace Spoon
{
    struct TextComp : public Component
    {
        TextComp(sf::Font& asset, std::string text = "", unsigned int char_size = 30, sf::Color color = sf::Color::White, bool centered = true) 
            : m_Text(asset, text, char_size) 
        { 
            m_Text.setFillColor(color);
            if (centered) { CenterOrigin(); }
        }

        sf::Text m_Text;

        sf::Text& GetText() { return m_Text; }

        void SetText(std::string text) { m_Text.setString(text); }
        void SetFont(sf::Font& font) { m_Text.setFont(font); }
        void SetSize(unsigned int size) { m_Text.setCharacterSize(size); }
        void SetPosition(sf::Vector2f pos) { m_Text.setPosition(pos); }
        void SetColor(sf::Color color) { m_Text.setFillColor(color); }
        void CenterOrigin() 
        {
            sf::FloatRect bounds = m_Text.getLocalBounds(); 
            m_Text.setOrigin({ bounds.size.x / 2.0f, bounds.size.y / 2.0f });
        }
        void SetAlpha(float alpha)
        {
            sf::Color& color = m_Text.getFillColor();
            color.a = static_cast<uint8_t>(alpha);
            m_Text.setFillColor(color);
        }
    };
}