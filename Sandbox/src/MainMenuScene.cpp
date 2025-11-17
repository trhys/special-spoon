#include "MainMenuScene.h"
#include "DemoZombie.h"

MainMenu::MainMenu() : Spoon::Scene("MainMenu", {1080, 1080})
{
    LoadTextures(m_Assets);
    AddChild(new Spoon::Entity(m_Assets["main_menu"]));
    AddChild(new ZombieSpawner({0, 800});
}

void MainMenu::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 5)
    {
        timer = timer.Zero;
    }
}

sf::Texture& MainMenu::GetAsset(std::string id)
{
    return m_Assets[id];
}