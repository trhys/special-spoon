#pragma once

#include "Spoon.h"

class DemoZombie : public Spoon::Entity
{
public:
    DemoZombie(sf::Texture& texture) : Entity::Entity(texture) {}
    ~DemoZombie() {}

    void OnAdd() override {}
    void OnKill() override;
    void OnCollision() override;

private:
    sf::Vector2f m_CurrentPosition;
    float speed = 1.0;
    sf::Time timer;

    void OnUpdate(sf::Time tick) override;
};

class ZombieSpawner : public Spoon::Node
{
public:
    ZombieSpawner(sf::Vector2f point) { setPosition(point); }
    ~ZombieSpawner() { for(auto& child : GetChildren()) delete child; }

    void SpawnZombie();

private:
    sf::Time timer;
    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}
    void OnUpdate(sf::Time tick) override;
};

