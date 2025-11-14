#include "DemoLayer.h"
#include "Background.h"
#include "DemoZombie.h"


DemoLayer::DemoLayer()
{
    SetSceneBounds({0, 0}, {600, 600});

}

void DemoLayer::OnAttach()
{

}

void DemoLayer::OnUpdate(sf::Time tick)
{
    ProcessBuffer();
    UpdateScene(tick, this);
}

bool DemoLayer::OnEvent(const sf::Event& e)
{
    if(const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Enter)
        {
            if(GetIsActive()) { HideScene(); }
            else if (!GetIsActive()) { ShowScene(); StartDemo(); }
        }
    }

    return false;
}

void DemoLayer::StartDemo()
{
    //CreateNode<Background>(Spoon::Layer::GetTexture("background", "resources/TD-Scene.png"));
    //CreateNode<ZombieSpawner>(sf::Vector2f{ 200, 200 });
    Spoon::Layer::testt();
}