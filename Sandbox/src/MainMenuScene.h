#pragma once

#include "Spoon.h"

class MainMenu : public Spoon::Scene
{
public:
    MainMenu();
    ~MainMenu() {}

    void OnTransition() override;
    void OnCache() override;

private:
    void OnUpdate(sf::Time tick) override;

    sf::Time timer;
    bool transitioning = false;
};

class MenuText : public Spoon::Node
{
public:
    MenuText(sf::Font& font) : text(font, "Press Enter to Start") {}
    ~MenuText() {}

private:
    sf::Text text;
    sf::Color textcolor = sf::Color(255, 255, 255, 255);
    bool flicker = false;

    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(text, states);
    }

    void OnUpdate(sf::Time tick) override
    {
        if(!flicker)
        {
            textcolor.a -= 127.5 * tick.asSeconds();
            if(textcolor.a == 0) { flicker = true; }
        }
        if(flicker)
        {
            textcolor.a += 127.5 * tick.asSeconds();
            if(textcolor.a == 255) { flicker = false; }
        }
        text.setFillColor(textcolor);
    }
};