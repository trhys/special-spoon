#pragma once

#include "Spoon.h"

// defines area that can be hit by attacks
// checked for overlap in combat system and resolve damage taken
struct HurtboxComp : public Spoon::ComponentBase<HurtboxComp> {
    HurtboxComp() : Spoon::ComponentBase<HurtboxComp>(Name) {}

    static constexpr const char* Name = "Hurtbox";

    sf::FloatRect box{};                   // world-space AABB
    bool enabled = true;                   // active hurtbox toggle

    void OnReflect() override
    {
        ImGui::SliderFloat2("Hurtbox Size", &box.size.x, 0.0f, 1000.0f);
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HurtboxComp, box, enabled)