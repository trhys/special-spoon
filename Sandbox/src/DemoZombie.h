#pragma once

#include "Spoon.h"

class DemoZombie : public Spoon::Entity
{
public:
    DemoZombie(sf::Texture& texture, sf::Vector2f position, bool wander = true);
    ~DemoZombie() {}

    void CollisionDetected() override;

private:
    sf::Time timer;
    bool is_Wandering;

private:
    void OnUpdate(sf::Time tick) override;
};