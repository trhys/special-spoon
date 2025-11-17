#pragma once

#include "Spoon.h"

class DemoZombie : public Spoon::Entity
{
public:
    DemoZombie(sf::Texture& texture) : Entity::Entity(texture) {}
    ~DemoZombie() {}

    void OnAdd() override {}
    void OnKill() override {}

private:
    sf::Vector2f speed = { 1, 0 };
    void OnUpdate(sf::Time tick) override;
};

class ZombieSpawner : public Spoon::Node
{
public:
    ZombieSpawner(sf::Vector2f point) { setPosition(point); }
    ~ZombieSpawner() {}

    void SpawnZombie();

private:
    sf::Time timer;
    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}
    void OnUpdate(sf::Time tick) override;
};

