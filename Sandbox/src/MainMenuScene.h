#pragma once

#include "Spoon.h"

class MainMenu : public Spoon::Scene
{
public:
    MainMenu();
    ~MainMenu() {}

    void OnTransition() override;
    void OnCache() override;
    void OnStart() override;
    void OnEnd() override;

private:
    void OnUpdate(sf::Time tick) override;

    sf::Time timer;
    bool transitioning = false;
    bool is_Initialized = false;
};

class MenuText : public Spoon::Node
{
public:
    MenuText(sf::Font& font, sf::Vector2f position = { 0,0 }) : text(font, "Press Enter to Start") { setPosition(position); }
    ~MenuText() {}

private:
    sf::Text text;
    sf::Color textcolor = sf::Color(255, 255, 255, 255);
    float text_alpha = 255.0f;
    bool flicker = true;

    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(text, states);
    }

    void OnUpdate(sf::Time tick) override
    {   
        if (text_alpha >= 255.0f) 
        { 
            text_alpha = 254.0f;
            flicker = true;
        }
        else if (text_alpha <= 0.0f)
        {
            text_alpha = 1.0f;
            flicker = false;
        }
        if (flicker)
        {
            text_alpha -= 127.5f * tick.asSeconds();
            textcolor.a = static_cast<std::uint8_t>(text_alpha);
        }
        else if (!flicker)
        {
            text_alpha += 127.5f * tick.asSeconds();
            textcolor.a = static_cast<std::uint8_t>(text_alpha);
        }
        text.setFillColor(textcolor);
    }
};