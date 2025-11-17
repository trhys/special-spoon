#pragma once

#include "Spoon.h"

#include <iostream>

sf::Texture texture("resources/logo.png");

class TestEntity : public Spoon::Entity
{
public:
    TestEntity()
        : Spoon::Entity(texture)
    {
    }

    void OnAdd() override
    {
        std::cout << "added entity (also testing post build commands)";
    }

    void OnKill() override
    {
        std::cout << "entity killed";
    }
};
