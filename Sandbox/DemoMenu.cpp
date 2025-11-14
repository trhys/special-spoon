#include "DemoMenu.h"

bool DemoMenu::OnEvent(sf::Event& e)
{
    if (const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Enter)
        {
            if (GetIsActive()) { HideScene(); }
        }
    }

    return false;
}