#pragma once

#include "Component.h"

#include "SFML/Graphics/Text.hpp"

namespace Spoon
{
    class TextComp : public Component
    {
    public:
        TextComp(sf::Font& asset, std::string text = "", sf::Vector2f position = {0.0f, 0.0f}, bool blinking = false) : m_Text(asset, text), is_Blinking(blinking)
        {
            m_Text.setPosition(position);
        }
        ~TextComp() {}

        void Update(sf::Time tick) override;

        void ToggleBlink()
        void SetBlinkRate(float rate);
        void SetColor(sf::Color color);

        sf::Text& GetText() { return m_Text; }

    private:
        sf::Text m_Text;
        sf::Color m_Color;
        float m_Alpha = 255.0f;
        float m_Blinkrate = 2.0f;
        sf::Time m_Timer;

        bool is_Blinking;
        bool blink_switch = false;
    };
}