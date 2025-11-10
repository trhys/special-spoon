#pragma once

#include "Spoon.h"

class DemoZombie : public Spoon::Entity
{
public:
    DemoZombie(sf::Texture& texture) {}
    ~DemoZombie() {}

    void OnAdd() override {}
    void OnKill() override {}

private:
    int speed = 1;
    void OnUpdate(sf::Time tick, Layer* context) override;
};

class ZombieSpawner : public Spoon::Node
{
public:
    ZombieSpawner(sf::Vector2f point) { setPosition(point); }
    ~ZombieSpawner() {}

    void SpawnZombie(Layer* context);

private:
    sf::Time timer;
    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}
    void OnUpdate(sf::Time tick, Layer* context) override;
};

