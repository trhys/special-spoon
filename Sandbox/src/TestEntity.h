#pragma once

#include "Spoon.h"
#include <iostream>

class TestEntity : public Spoon::Entity
{
public:
    TestEntity() : m_Texture({300, 300}), m_Sprite(m_Texture) {}

    void OnAdd() override
    {
        std::cout << "added entity";
    }

    void OnKill() override
    {
        std::cout << "entity killed";
    }
};
