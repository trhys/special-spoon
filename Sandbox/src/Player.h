#pragma once

#include "Spoon.h"

class Player : public Spoon::Entity
{
public:
    Player(sf::Texture& texture) : Entity::Entity(texture, true);
    ~Player() {}

    void TurnLeft(sf::Time tick);
    void TurnRight(sf::Time tick);
    void TurnUp(sf::Time tick);
    void TurnDown(sf::Time tick);

    void OnUpdate(sf::Time tick) override;
    void CollisionDetected() override;

private:
    sf::Vector2f m_CurrentPosition;
    float m_Speed = 100;
};