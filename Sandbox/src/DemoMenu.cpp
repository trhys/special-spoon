#include "DemoMenu.h"
#include "MainMenuScene.h"
#include "DemoLayer.h"
#include "SceneTransition.h"

void DemoMenu::OnAttach()
{
    RequestScene("main_menu", new MainMenu());
    BeginScene("main_menu");
}

void DemoMenu::OnDetach()
{
    Spoon::Application::Get().PushLayer(new DemoLayer());
    delete this;
}

void DemoMenu::OnUpdate(sf::Time tick)
{
    if(transitiontimer)
    {
        timer = timer + tick;
        if(timer.asSeconds() > 3)
        {
            timer = timer.Zero;
            transitiontimer = false;
            EndScene();
            Spoon::Application::Get().PopLayer(this);
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
            Spoon::Application::Get().PushLayer(new LoadScreen());
            return true;
        }
    }

    return false;
}