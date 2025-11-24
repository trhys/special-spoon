#include "SceneTransition.h"

Transition::Transition() : Spoon::Scene("loadscreen", {1080, 1080})
{
    rect.setSize({ 1080, 1080 });
}

void Transition::OnCache()
{

}

void Transition::OnTransition()
{

}

void LoadScreen::OnAttach()
{
    RequestScene("loadscreen", new Transition());
    PushOverlay("loadscreen");
}

void LoadScreen::OnDetach()
{
    delete this;
}

void LoadScreen::OnUpdate(sf::Time tick)
{
    timer = timer + tick;
    if(timer.asSeconds() > 5)
    {
        PopOverlay();
        Spoon::Application::Get().PopLayer(this);
    }
}