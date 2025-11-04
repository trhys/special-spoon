#include "Spoon.h"
#include <iostream>

class TestEntity : public Spoon::Entity
{
public:
    TestEntity()
    {
        //m_Sprite.setTexture(m_Texture);
        //m_Sprite.setColor(sf::Color::Green);
    }

    virtual void OnAdd() override
    {
        std::cout << "added entity";
    }

    virtual void OnKill() override
    {
        std::cout << "entity killed";
    }
};
