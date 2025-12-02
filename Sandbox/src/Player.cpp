#include "Player.h"
#include "iostream"

Player::Player(sf::Texture& texture)
{
    AddComponent<TransComp>({540.0f 540.0f});
    AddComponent<SpriteComp>(texture);
    GetComponent<SpriteComp>()->CenterOrigin();
    GetComponent<SpriteComp>()->SetScale({ 0.25, 0.25 });
    AddComponent<PhysComp>(GetComponent<SpriteComp>()->GetBoundingBox());
}
void Player::TurnLeft(sf::Time tick)
{
    GetComponent<TransComp>()->Move({-1 * (tick.asSeconds() * 100.0f), 0});
    ScaleSprite({-0.25, 0.25});
}

void Player::TurnRight(sf::Time tick)
{
    GetComponent<TransComp>()->Move({1 * (tick.asSeconds() * 100.0f), 0});
    ScaleSprite({0.25, 0.25});
}

void Player::TurnUp(sf::Time tick)
{
    GetComponent<TransComp>()->Move({0, -1 * (tick.asSeconds() * 100.0f)});
}

void Player::TurnDown(sf::Time tick)
{
    GetComponent<TransComp>()->Move({0, 1 * (tick.asSeconds() * 100.0f)});
}

void Player::CollisionDetected()
{
    GetComponent<TransComp>()->ResetPos();
    std::cout << "COLLISION DETECTED" << std::endl;
}

void Player::OnUpdate(sf::Time tick)
{    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { TurnUp(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { TurnLeft(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { TurnDown(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { TurnRight(tick); }
}