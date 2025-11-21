#include "DemoScene.h"
#include "DemoZombie.h"
#include "Player.h"
#include "SceneTransition.h"

DemoScene::DemoScene() : Spoon::Scene("demo_scene", {1080, 1080})
{
    GetView() = (Spoon::Application::Get().GetWindow().getDefaultView());
}

void DemoScene::OnCache()
{
    AddChild(new Spoon::Entity(LoadTexture("demoscene", "resources/TD-Scene.png")));
    AddChild(new ZombieSpawner({500, 500}));
    AddChild(new ZombieSpawner({700, 700}));
    AddChild(new Player(LoadTexture("player", "resources/Player.png")));
}

void DemoScene::OnTransition()
{
    AddChild(new Transition(GetBounds().size));
    transitioning = true;
}

void DemoScene::OnUpdate(sf::Time tick)
{
    if(transitioning)
    {
        timer = timer + tick;
        if(timer.asSeconds() > 3)
        {
            for(auto& child : GetChildren())
            {
                KillChild(child);
            }
        }
    }
}