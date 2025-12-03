#include "TextComp.h"

namespace Spoon
{
    void TextComp::Update(sf::Time tick)
    {
        if(is_Blinking)
        {
            m_Timer = m_Timer + tick;
            float progress = std::min(m_Timer.asSeconds() / m_Blinkrate, 1.0f);
            
            if(!blink_switch)
            {
                m_Alpha = 255.0f + (0.0f - 255.0f) * progress;
                m_Color.a = static_cast<std::uint8_t>(m_Alpha);
            }
            else
            {
                m_Alpha = 255.0f + (255.0f - 0.0f) * progress;
                m_Color.a = static_cast<std::uint8_t>(m_Alpha);
            }

            if(progress >= 255.0f)
            {
                blink_switch = !blink_switch;
                m_Timer.Zero;
            }

            m_Text.setFillColor(m_Color);
        }
    }

    void TextComp::ToggleBlink()
    {
        if(is_Blinking) 
        { 
            is_Blinking = false;
            m_Alpha = 255.0f;
            m_Color.a = static_cast<std::uint8_t>(m_Alpha);
            m_Text.setFillColor(m_Color);
        }
        else { is_Blinking = true; }
    }

    void TextComp::SetBlinkRate(float rate)
    {
        m_Blinkrate = rate;
    }

    void TextComp::SetColor(sf::Color color)
    {
        m_Color = color;
    }
}