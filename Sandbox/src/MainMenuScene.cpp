#include "MainMenuScene.h"
#include "DemoZombie.h"

MainMenu::MainMenu() : Spoon::Scene("MainMenu", {1080, 1080})
{
    AddChild(new Spoon::Entity(LoadTexture("menu_screen", "resources/SV-Scene.png")));
    AddChild(new ZombieSpawner({0, 800}));
}

void MainMenu::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 5)
    {
        timer = timer.Zero;
    }
}