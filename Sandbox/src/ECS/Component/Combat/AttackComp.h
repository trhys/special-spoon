#pragma once 

#include "Spoon.h"

struct AttackComp : public Spoon::ComponentBase<AttackComp>
{
    AttackComp() : Spoon::ComponentBase<AttackComp>(AttackComp::Name) {}

    static constexpr const char* Name = "Attack";

    std::string selectedAttack;  // the name of the currently selected attack
    bool isAttacking = false;

    void OnReflect() override
    {

    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AttackComp, selectedAttack)