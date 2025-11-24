#pragma once

#include "Spoon.h"

class DemoLayer : public Spoon::Layer
{
public:
    DemoLayer();
    ~DemoLayer() {}

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(sf::Time tick) override;
    bool OnEvent(const sf::Event& e) override;

private:
    sf::Time timer;
    bool transitioning = false;
};