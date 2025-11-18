#include "DemoMenu.h"
#include "MainMenuScene.h"
#include "DemoScene.h"
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
    GetSM()->UpdateScene(tick);

    if(transitiontimer)
    {
        timer = timer + tick;
        if(timer.asSeconds() > 5)
        {
            timer = timer.Zero;
            transitiontimer = false;
            EndScene();
            StartDemo();
        }
    }
}

bool DemoMenu::OnEvent(const sf::Event& e)
{
    if (const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Enter)
        {
            GetSM()->TransitionScene();
            transitiontimer = true;
        }
    }

    return false;
}

void DemoMenu::StartDemo()
{
    RequestScene("demoscene", new DemoScene());
    BeginScene("demoscene");
}