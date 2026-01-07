#pragma once 

#include "Spoon.h"

struct AttackComp : public Spoon::ComponentBase
{
    AttackComp() : Spoon::ComponentBase::ComponentBase(AttackComp::Name) {}

    static constexpr const char* Name = "Attack";

    // Core members
    float m_ReloadTime = 0.0f;
    std::vector<Hitbox> m_Hitboxes;

    // Controls
    float reloadTimer = 0.0f;
    bool isAttacking = false;

    void OnReflect() override
    {
        if (ImGui::Sliderfloat("Reload Time", &m_ReloadTime, 0.f, 10.f)) {}
    }
};