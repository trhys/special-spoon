#include "Player.h"

Player::Player(sf::Texture& texture) : Entity::Entity(texture, true)
{
    CenterOrigin();
    ScaleSprite({ 0.25, 0.25 });
}
void Player::TurnLeft(sf::Time tick)
{
    move({-1 * (tick.asSeconds() * m_Speed), 0});
    ScaleSprite({-0.25, 0.25});
}

void Player::TurnRight(sf::Time tick)
{
    move({1 * (tick.asSeconds() * m_Speed), 0});
    ScaleSprite({0.25, 0.25});
}

void Player::TurnUp(sf::Time tick)
{
    // TODO TURN SPRITE UP
    move({0, -1 * (tick.asSeconds() * m_Speed)});
}

void Player::TurnDown(sf::Time tick)
{
    // TODO TURN SPRITE DOWN
    move({0, 1 * (tick.asSeconds() * m_Speed)});
}

void Player::CollisionDetected()
{
    setPosition(m_CurrentPosition);
}

void Player::OnUpdate(sf::Time tick)
{
    m_CurrentPosition = getPosition();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { TurnUp(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { TurnLeft(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { TurnDown(tick); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { TurnRight(tick); }
}