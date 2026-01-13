#pragma once

#include "Spoon.h"

struct Hitbox
{
    Hitbox(sf::FloatRect box = sf::FloatRect(), float damage = 0.0f, float knockback = 0.0f, float lifetime = 0.0f)
        : box(box), damage(damage), knockback(knockback), lifetime(lifetime) {}

    // Core properties
    sf::FloatRect box;
    float damage;
    float knockback;
    float lifetime;

    // Runtime properties
    Spoon::UUID owner;
    std::vector<Spoon::UUID> hitEntities;
};

struct Hurtbox
{
    std::vector<sf::FloatRect> boxes;
    Spoon::UUID owner;
    bool isInvincible;
};