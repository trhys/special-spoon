#pragma once

#include "Spoon.h"

class DemoZombie : public Spoon::Entity
{
public:
    DemoZombie(sf::Texture& texture) : Entity::Entity(texture, true) { ScaleSprite({ 0.25, 0.25 }); }
    ~DemoZombie() {}

    void OnKill() override;
    void CollisionDetected() override;

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

class MenuZombie : public Spoon::Entity
{
public:
    MenuZombie(sf::Texture& texture) : Entity::Entity(texture) { ScaleSprite({ 0.25, 0.25 }); }
    ~MenuZombie() {}

    void OnKill() override;

private:
    sf::Time timer;

    void OnUpdate(sf::Time tick) override;
};

class MenuZombieSpawner : public Spoon::Node
{
public:
    MenuZombieSpawner(sf::Vector2f point) { setPosition(point); }
    ~MenuZombieSpawner() { for(auto& child : GetChildren()) delete child; }

    void OnAdd() override;
    void SpawnMenuZombie();

private:
    sf::Time timer;
    void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}
    void OnUpdate(sf::Time tick) override;
};

