#include "DemoScene.h"
#include "DemoZombie.h"
#include "Player.h"

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
    AddChild<Arena>(Spoon::ResourceManager::GET_TEXTURE("demoscene"));
    AddChild<DemoZombie>(Spoon::ResourceManager::GET_TEXTURE("demozombie"), sf::Vector2f{500.0f, 500.0f});
    AddChild<DemoZombie>(Spoon::ResourceManager::GET_TEXTURE("demozombie"), sf::Vector2f{200.0f, 800.0f});
    AddChild<Player>(Spoon::ResourceManager::GET_TEXTURE("player"));
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