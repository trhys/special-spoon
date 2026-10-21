#include "Spoon.h"

#include "System/CombatSystem.h"
#include "System/HealthSystem.h"

std::unique_ptr<Spoon::ISystem> LoadCombatSystem(const json* systemData)
{
    return std::make_unique<CombatSystem>();
}

std::unique_ptr<Spoon::ISystem> LoadHealthSystem(const json* systemData)
{
    return std::make_unique<HealthSystem>();
}

void RegisterCustomSystems()
{
    Spoon::SystemRegistry::Get().RegisterLoader("Combat", &LoadCombatSystem);
    Spoon::SystemRegistry::Get().RegisterLoader("Health", &LoadHealthSystem);
}
