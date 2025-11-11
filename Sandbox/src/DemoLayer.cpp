#include "DemoLayer.h"
#include "Background.h"
#include "DemoZombie.h"


DemoLayer::DemoLayer()
{
    SetSceneBounds({0, 0}, {600, 600});
    AddSceneNode(new Background(Spoon::Layer::GetTexture("background", "resources/TD-Scene,png")));
    AddSceneNode(new ZombieSpawner({200, 200}));
}

void DemoLayer::OnAttach()
{

}

bool DemoLayer::OnEvent(const sf::Event& e)
{
    if(const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Enter)
        {
            if(GetIsActive()) { HideScene(); }
            else if(!GetIsActive()) { ShowScene(); }
        }
    }

    return false;
}