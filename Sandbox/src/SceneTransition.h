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
    float alpha_value = 0.0f;

    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(rect, states);
    }

    void OnUpdate(sf::Time tick) override
    {
        if(alpha_value < 255.0f)
        {
            alpha_value += 127.5f * tick.asSeconds();
            rectcolor.a = static_cast<std::uint8_t>(alpha_value);
            rect.setFillColor(rectcolor);
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