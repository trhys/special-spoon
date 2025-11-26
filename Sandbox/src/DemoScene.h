#pragma once

#include "Spoon.h"

class DemoScene : public Spoon::Scene
{
public:
    DemoScene();
    ~DemoScene() {}

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