#pragma once

#include "Spoon.h"

// defines area that can be hit by attacks
// checked for overlap in combat system and resolve damage taken
struct HurtboxComp : public Spoon::ComponentBase<HurtboxComp> {
    HurtboxComp() : Spoon::ComponentBase<HurtboxComp>(Name) {}

    static constexpr const char* Name = "Hurtbox";

    sf::FloatRect box{};                   // world-space AABB
    bool enabled = true;                   // active hurtbox toggle
    bool isInvincible = false;             // temp immunity gate

    void OnReflect() override
    {

    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HurtboxComp, box, enabled, isInvincible)