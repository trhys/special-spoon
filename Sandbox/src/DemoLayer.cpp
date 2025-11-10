#include "DemoLayer.h"

DemoLayer::DemoLayer()
{
    Layer::SetScene(new DemoScene());
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