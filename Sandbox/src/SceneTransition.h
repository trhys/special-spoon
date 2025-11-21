#pragma once

#include "Spoon.h"

class Transition : public Spoon::Scene
{
public:
    Transition();
    ~Transition() {}

    void OnTransition() override;
    void OnCache() override;

private:
    sf::RectangleShape rect;
    sf::Color rectcolor = sf::Color(0, 0, 0, 0);

    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(rect, states);
    }

    void OnUpdate(sf::Time tick) override
    {
        if(rectcolor.a < 255)
        {
            rectcolor.a += 127.5 * tick.asSeconds();
        }
        else if(rectcolor.a > 255)
        {
            rectcolor.a = 255;
        }
    }
};

class LoadScreen : public Spoon::Layer
{
public:
    LoadScreen() {}
    ~LoadScreen() {}

    void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(sf::Time tick) override;
    
private:
    sf::Time timer;
};