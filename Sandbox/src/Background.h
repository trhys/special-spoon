#pragma once

#include "Spoon.h"

#include "SFML/Graphics.hpp"

class Background : public Spoon::Node
{
public:
    Background(sf::Texture& background) : m_Sprite(background)
    {

    }
    ~Background() 
    {

    }

private:
    sf::Sprite m_Sprite;

    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(m_Sprite, states);
    }
};