#include "DemoLayer.h"

void DemoLayer::DemoLayer()
{
    
}

bool DemoLayer::OnEvent(const sf::Event& e)
{
    if(e->sf::GetIf<sf::Event::KeyPressed::code == Tab)
    {
        m_Scene->AddChild();
    }
}