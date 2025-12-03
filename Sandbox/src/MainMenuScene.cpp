#include "MainMenuScene.h"

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
        GetView() = Spoon::Application::Get().GetWindow().getDefaultView();
        GetView().zoom(0.8);

        AddChild<MenuScreen>(Spoon::ResourceManager::GET_TEXTURE("menu_screen"));
        AddChild<DemoZombie>(Spoon::ResourceManager::GET_TEXTURE("demozombie"), sf::Vector2f{-100.0f, 600.0f}, false);
        AddChild<MenuText>(Spoon::ResourceManager::GET_FONT("menu_text"));
        
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
        float transition_Progress = std::min(tick.asSeconds() / 2.0f, 1.0f);
        float startZoom = 1.0f;
        float targetZoom = 2.0f;
        float currentZoom = startZoom + (targetZoom - startZoom) * transition_Progress;

        GetView().zoom(currentZoom);
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