#include "TestLayer.h"
#include "TestEntity.h"

void TestLayer::OnAttach()
{
   TestLayer::AddEntity(new TestEntity());
}

bool TestLayer::OnEvent(const sf::Event& event)
{
    using T = std::decay_t<decltype(event)>;

    if constexpr (std::is_same_v<T, sf::Event::KeyPressed>)
    {
        if(event.code == sf::KeyBoard::Key::Enter)
        {
            m_Entities[0].setColor(sf::Color::White);
            return true;
        }
        else if(event.code == sf::KeyBoard::Key::Tab)
        {
            m_Entities[0].setColor(sf::Color::Red);
            return true;
        }
        else if(event.code == sf::KeyBoard::Key::Backspace)
        {
            KillEntity(m_Entities[0]);
            return true;
        }
        else { return false; }
    }

    return false;
}