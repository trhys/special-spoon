#pragma once

#include "Spoon.h"

class MainMenu : public Spoon::Scene
{
public:
    MainMenu();
    ~MainMenu() {}


private:
    void OnUpdate(sf::Time tick) override;

    sf::Time timer;
};