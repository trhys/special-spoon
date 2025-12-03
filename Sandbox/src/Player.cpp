#include "Player.h"
#include "iostream"

Player::Player(sf::Texture& texture)
{
    AddComponent<Spoon::TransComp>(sf::Vector2f{540.0f, 540.0f});
    AddComponent<Spoon::SpriteComp>(texture);
    GetComponent<Spoon::SpriteComp>()->CenterOrigin();
    GetComponent<Spoon::SpriteComp>()->SetScale({ 0.25, 0.25 });
    AddComponent<Spoon::PhysComp>(GetComponent<Spoon::SpriteComp>()->GetBoundingBox());
}
void Player::TurnLeft(sf::Time tick)
{
    GetComponent<Spoon::TransComp>()->Move({-1 * (tick.asSeconds() * 100.0f), 0});
    GetComponent<Spoon::SpriteComp>()->SetScale(sf::Vector2f{-0.25, 0.25});
}

void Player::TurnRight(sf::Time tick)
{
    GetComponent<Spoon::TransComp>()->Move({1 * (tick.asSeconds() * 100.0f), 0});
    GetComponent<Spoon::SpriteComp>()->SetScale({0.25, 0.25});
}

void Player::TurnUp(sf::Time tick)
{
    GetComponent<Spoon::TransComp>()->Move({0, -1 * (tick.asSeconds() * 100.0f)});
}

void Player::TurnDown(sf::Time tick)
{
    GetComponent<Spoon::TransComp>()->Move({0, 1 * (tick.asSeconds() * 100.0f)});
}

void Player::CollisionDetected()
{
    GetComponent<Spoon::TransComp>()->ResetPos();
    std::cout << "COLLISION DETECTED" << std::endl;
}

void Player::OnUpdate(sf::Time tick)
{    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { TurnUp(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { TurnLeft(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { TurnDown(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { TurnRight(tick); }
}