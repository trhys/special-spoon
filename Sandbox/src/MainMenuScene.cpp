#include "MainMenuScene.h"
#include "SceneTransition.h"
#include "DemoZombie.h"

MainMenu::MainMenu() : Spoon::Scene("MainMenu", {1080, 1080})
{
    GetView().zoom(0.85);
}

void MainMenu::OnCache()
{
    AddChild(new Spoon::Entity(LoadTexture("menu_screen", "resources/SV-Scene.png")));
    AddChild(new MenuZombieSpawner({0, 800}));
    AddChild(new MenuText(LoadFont("menu_text", "resources/Fonts/RoadRage/RoadRage-Regular.ttf")), {GetBounds().size.x / 2, GetBounds().size.y / 2 });
}

void MainMenu::OnUpdate(sf::Time tick)
{
    if(transitioning)
    {
        timer = timer + tick;
        GetView().zoom(1.15 * timer.asSeconds());
        if(timer.asSeconds() > 3)
        {
            for(auto& child : GetChildren())
            {
                KillChild(child);
            }
        }
    }
}

void MainMenu::OnTransition()
{
    auto& children = GetChildren();
    for(auto it = children.begin(); it != children.end(); ++it)
    {
        MenuText* finder = dynamic_cast<MenuText*>(*it);
        if(finder)
        {
            KillChild(finder);
            break;
        }
    }
    AddChild(new Transition(GetBounds().size));
    transitioning = true;
}