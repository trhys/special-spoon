#pragma once

#include "Spoon.h"

// authored attack definition
struct AttackDef {
    std::string id;                        // unique key, e.g. "light_slash"
    std::string name;                      // display name

    // hitbox dims
    sf::Vector2f halfExtents{ 16.f, 8.f }; // half-size of hitbox (box shape)
    sf::Vector2f offset{ 24.f, 0.f };      // local offset from attacker origin/facing

    // timing for the attack phases
    // sets up combo implementation
    float windupSec = 0.05f;
    float activeSec = 0.10f;
    float recoverySec = 0.20f;
    float cooldownSec = 0.20f;

    float damage = 0.f;
    float knockback = 0.f;
    float lifetimeSec = 0.f;
    int hitsPerTarget = 1;                  // usually 1 for melee
    bool friendlyFire = false;
    bool canHitSelf = false;
    bool followOwner = true;                // if true, hitbox tracks owner while active
};

// runtime attack hitbox checked by combat system
// for overlap and resolves damage dealt
// constructed from the authored AttackDef struct
struct Hitbox {
    std::uint64_t attackInstanceId = 0; // unique per execution/swing
    Spoon::UUID owner = 0;              // source entity

    sf::FloatRect box{};                // world-space AABB
    float lifetimeSec = 0.f;                
    float damage = 0.f;
    float knockback = 0.f;

    int hitsPerTarget = 1;
    bool friendlyFire = false;
    bool canHitSelf = false;
    bool followOwner = true;

    // deduplicated set of entities that have been hit by this hitbox
    std::unordered_set<Spoon::UUID> hitEntities;

    // constructor (from AttackDef)
    Hitbox(const AttackDef& attack)
        : damage(attack.damage),
          knockback(attack.knockback),
          lifetimeSec(attack.lifetimeSec),
          hitsPerTarget(attack.hitsPerTarget),
          friendlyFire(attack.friendlyFire),
          canHitSelf(attack.canHitSelf),
          followOwner(attack.followOwner)
    {
    }

    void AddHit(Spoon::UUID id) {
        hitEntities.insert(id);
    }
};


