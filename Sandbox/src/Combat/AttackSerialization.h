#pragma once

#include "Spoon.h"

// forward declarations
struct AttackDef;
struct AttackComp;

// Functions for managing attack serialization and validation
// Used by AttackComp for the editor tool, as well as
// AttackRegistry for managing loaded attacks
namespace Attack {
    std::filesystem::path GetAttacksDirectory();
    std::vector<std::string> GetAttackIds();
    bool ValidateAttack(const AttackDef& attack, AttackComp& comp);
    void SaveAttack(const AttackDef& attack, AttackComp& comp);
    void LoadAttack(const std::string& id, AttackComp& comp, AttackDef& attack);
    void DeleteAttack(const std::string& id);
}

// run at app construction to initialize 
// static registry of attacks
namespace init {
    void InitializeAttackRegistry();
}