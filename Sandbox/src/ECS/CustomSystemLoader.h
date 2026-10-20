#include "Spoon.h"

#include "System/CombatSystem.h"

std::unique_ptr<Spoon::ISystem> LoadCombatSystem(const json* systemData)
{
    return std::make_unique<CombatSystem>();
}

void RegisterCustomSystems()
{
    Spoon::SystemRegistry::Get().RegisterLoader("Combat", &LoadCombatSystem);
}
