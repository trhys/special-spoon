#pragma once

#include "Spoon.h"

class DemoZombie : public Spoon::Entity
{
public:
    DemoZombie(sf::Texture& zombie_texture);
    ~DemoZombie() {}

    void OnAdd() override;
    void OnKill() override;

};