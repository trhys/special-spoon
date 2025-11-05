#pragma once

#include "Spoon.h"
#include <iostream>

class TestEntity : public Spoon::Entity
{
public:
    TestEntity() 
        : Spoon::Entity(sf::Texture::Texture(sf::Vector2u{ 300, 300 }))
    {
    }

    void OnAdd() override
    {
        std::cout << "added entity";
    }

    void OnKill() override
    {
        std::cout << "entity killed";
    }
};
