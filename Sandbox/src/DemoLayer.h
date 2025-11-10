#pragma once

#include "Spoon.h"
#include "DemoScene.h"

class DemoLayer : public Spoon::Layer
{
public:
    DemoLayer();
    ~DemoLayer() {}

    bool OnEvent(const sf::Event& e) override;
};