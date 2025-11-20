#pragma once

#include "Spoon.h"

class Transition : public Spoon::Node
{
public:
    Transition(sf::Vector2f bounds) : rect(bounds) {}

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
    }
};