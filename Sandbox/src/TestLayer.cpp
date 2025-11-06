#include "TestEntity.h"
#include "TestLayer.h"

void TestLayer::OnAttach()
{
  AddEntity(new TestEntity());
}

bool TestLayer::OnEvent(const sf::Event& event)
{
    if(const auto& keypress = event->getIf<sf::Event::KeyPressed>())
    {
        if(keypress->code() == sf::Keyboard::Key::Enter)
        {
            GetEntities()[0]->GetSprite().setColor(sf::Color::White);
            return true;
        }
        else if(keypress->code() == sf::Keyboard::Key::Tab)
        {
            GetEntities()[0]->GetSprite().setColor(sf::Color::Red);
            return true;
        }
        else if(keypress->code() == sf::Keyboard::Key::Backspace)
        {
            KillEntity(GetEntities()[0]);
            return true;
        }
        else { return false; }
    }

    return false;
}