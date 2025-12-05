#pragma once

#include "ECS/Components/Component.h"
#include "SFML/Graphics/Text.hpp"

namespace Spoon
{
    struct TextComp : public Component
    {
        TextComp(sf::Font& asset, std::string text = "", sf::Color color = sf::Color::White) : m_Text(asset, text) { m_Text.setFillColor(color); }

        sf::Text m_Text;

        sf::Text& GetText() { return m_Text; }

        void SetText(std::string text) { m_Text.setString(text); }
        void SetFont(sf::Font& font) { m_Text.setFont(font); }
        void SetSize(unsigned int size) { m_Text.setCharacterSize(size); }
        void SetPosition(sf::Vector2f pos) { m_Text.setPosition(pos); }
        void SetColor(sf::Color color) { m_Text.setFillColor(color); }
    };
}