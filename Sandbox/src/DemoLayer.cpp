#include "DemoLayer.h"
#include "Background.h"
#include "DemoZombie.h"


DemoLayer::DemoLayer()
{

}

void DemoLayer::OnAttach()
{

}

void DemoLayer::OnUpdate(sf::Time tick)
{
}

bool DemoLayer::OnEvent(const sf::Event& e)
{
    if(const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Enter)
        {

        }
    }

    return false;
}