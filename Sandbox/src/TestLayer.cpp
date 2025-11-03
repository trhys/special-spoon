#include "TestLayer.h"
#include "TestEntity.h"

void TestLayer::OnAttach()
{
    TestLayer::AddEntity(new TestEntity());
}

void TestLayer::OnEvent(sf::Event& event)
{
    if(event->is<sf::Event::KeyPressed()->scancode == sf::Keyboard::Scancode::Enter)
    {
        m_Entities[0].setColor(sf::Color::White);
        break;
    }
    else if(event->is<sf::Event::KeyPressed()->scancode == sf::Keyboard::Scancode::Tab)
    {
        m_Entities[0].setColor(sf::Color::Red);
        break;
    }
    else if(event->is<sf::Event::KeyPressed()->scancode == sf::Keyboard::Scancode::Backspace)
    {
        TestLayer::KillEntity(m_Entities[0]);
    }
}



