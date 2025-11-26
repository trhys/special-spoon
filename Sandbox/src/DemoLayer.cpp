#include "DemoLayer.h"
#include "DemoScene.h"
#include "DemoMenu.h"
#include "SceneTransition.h"

DemoLayer::DemoLayer()
{

}

void DemoLayer::OnAttach()
{
    RequestScene("demoscene", new DemoScene());
    BeginScene("demoscene");
}

void DemoLayer::OnDetach()
{
    Spoon::Application::Get().PushLayer(new DemoMenu());
    delete this;
}

void DemoLayer::OnUpdate(sf::Time tick)
{
    GetSM()->UpdateScene(tick);

    if(transitioning)
    {
        timer = timer + tick;
        if(timer.asSeconds() > 3)
        {
            timer = timer.Zero;
            transitioning = false;
            EndScene();
            Spoon::Application::Get().PopLayer(this);
        }
    }
}

bool DemoLayer::OnEvent(const sf::Event& e)
{
    if(const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Escape)
        {
            GetSM()->TransitionScene();
            transitioning = true;
            Spoon::Application::Get().PushLayer(new LoadScreen());
            return true;
        }
    }

    return false;
}