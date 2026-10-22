#pragma once 

#include "Spoon.h"

// attack phase enum
enum class AttackPhase
{
    None,
    WindUp,
    Strike,
    Recovery
};

struct AttackComp : public Spoon::ComponentBase<AttackComp>
{
    AttackComp() : Spoon::ComponentBase<AttackComp>(Name) {}

    static constexpr const char* Name = "Attack";

    std::string selectedAttack;  // the name of the currently selected attack
    std::uint64_t attackId = 0;  // unique identifier for the attack instance
    AttackPhase currentPhase = AttackPhase::None;

    float phaseTimer = 0.0f;    // time elapsed since the attack started
    float cooldownTimer = 0.0f; // time remaining until the next attack can be initiated
    bool isAttacking = false;

    // editor interface
    bool showAttackEditor = false;
    bool showErrorMessage = false;
    std::string editorErrorMessage;
    void OnReflect() override;
    void DrawAttackEditor();
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AttackComp, selectedAttack)