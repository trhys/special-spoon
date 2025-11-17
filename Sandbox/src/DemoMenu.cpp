#include "DemoMenu.h"
#include "MainMenuScene.h"
#include "DemoZombie.h"

void DemoMenu::OnAttach()
{
    RequestScene("main_menu", new MainMenu());
    BeginScene("main_menu");
}

void DemoMenu::OnDetach()
{

}

void DemoMenu::OnUpdate(sf::Time tick)
{
    p_SM->UpdateScene(tick);
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