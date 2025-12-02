#include "MainMenuScene.h"
#include "SceneTransition.h"
#include "DemoZombie.h"

MainMenu::MainMenu() : Spoon::Scene("MainMenu", {1080, 1080})
{
    
}

void MainMenu::OnCache()
{
    LoadTexture("menu_screen", "resources/SV-Scene.png");
    LoadFont("menu_text", "resources/Fonts/RoadRage/RoadRage-Regular.ttf");
    LoadTexture("demozombie", "resources/DemoSprite.png");
}

void MainMenu::OnStart()
{
    if(!is_Initialized)
    {
        GetView().zoom(0.8);
        AddChild<Spoon::Entity>("menu_screen");
        AddChild<ZombieSpawner>(sf::Vector2f{100.0f, 600.0f});
        AddText<MenuText>("menu_text", {400.0f, 400.0f});
        is_Initialized = true;
    }
}

void MainMenu::OnEnd()
{
    for(auto& child : GetChildren())
    {
        child->OnKill();
    }
    transitioning = false;
    is_Initialized = false;
}

void MainMenu::OnUpdate(sf::Time tick)
{
    if(transitioning)
    {
        timer = timer + tick;
        //GetView().zoom(1.15 * timer.asSeconds()); <====== ISSUE IS HERE
    }
}

void MainMenu::OnTransition()
{   
    // auto& children = GetChildren();
    // for(auto it = children.begin(); it != children.end(); ++it)
    // {
    //    MenuText* finder = dynamic_cast<MenuText*>(*it);
    //    if(finder)
    //    {
    //        finder->OnKill();
    //        break;
    //    }
    // }
    transitioning = true;
}