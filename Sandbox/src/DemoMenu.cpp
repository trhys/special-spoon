#include "DemoMenu.h"
#include "Background.h"

void DemoMenu::OnAttach()
{
    RequestScene("main_menu", {1080, 1080});
    BeginScene("main_menu");
    RequestEntity<Background>("background", "resources/SV-Scene.png");
}

void DemoMenu::OnDetach()
{

}

void DemoMenu::OnUpdate(sf::Time tick)
{

}

bool DemoMenu::OnEvent(const sf::Event& e)
{
    if (const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Enter)
        {
            
        }
    }

    return false;
}