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
    LoadTexture("demoscene", "resources/TD-Scene.png");
    LoadTexture("player", "resources/Player.png");
}

void DemoScene::OnStart()
{
    AddChild<Spoon::Entity, sf::Texture>("demoscene");
    AddChild<ZombieSpawner>({500, 500});
    AddChild<ZombieSpawner>({700, 700});
    AddChild<Player, sf::Texture>("player");
}

void DemoScene::OnEnd()
{
    for(auto& child : GetChildren())
    {
        child->OnKill();
    }
    is_Initialized = false;
}

void DemoScene::OnTransition()
{
    
}

void DemoScene::OnUpdate(sf::Time tick)
{

}