#pragma once

#include "Spoon.h"
#include "DemoScene.h"

class DemoLayer : public Spoon::Layer
{
public:
    DemoLayer() : m_Scene(DemoScene());
    ~DemoLayer() {};

    bool OnEvent(const sf::Event& e) override;
};