#pragma once

#include "Spoon.h"

class MainMenu : public Spoon::Scene
{
public:
    MainMenu();
    ~MainMenu() {}

    sf::Texture& GetAsset(std::string id);

private:
    void OnUpdate(sf::Time tick) override;

    std::unordered_map<std::string, sf::Texture> m_Assets = {
        {"menu_screen", sf::Texture("resources/SV-Scene.png")},
        {"demozombie", sf::Texture("resources/DemoSprite.png")}
    };

    sf::Time timer;
};