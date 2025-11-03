#include "Spoon.h"

class TestEntity : public Entity
{
public:
    TestEntity() 
    {
        m_Sprite.setColor(sf::Color::Green);
    }

    virtual TestEntity::OnAdd() override
    {
        std::cout << "added entity";
    }

    virtual TestEntity::OnKill() override
    {
        std::cout << "entity killed";
    }
}
