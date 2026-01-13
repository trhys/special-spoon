#include "Spoon.h"

#include "System/PatrolSystem.h"
#include "System/MovementSystem.h"
#include "System/PlayerController.h"

std::unique_ptr<Spoon::ISystem> LoadPatrolSystem(const nlohmann::json* systemData)
{
    return std::make_unique<PatrolSystem>();
}

std::unique_ptr<Spoon::ISystem> LoadMovementSystem(const nlohmann::json* systemData)
{
    return std::make_unique<MovementSystem>();
}

std::unique_ptr<Spoon::ISystem> LoadPlayerController(const nlohmann::json* systemData)
{
    return std::make_unique<PlayerController>();
}

void RegisterCustomSystems()
{
    Spoon::SystemRegistry::Get().RegisterLoader("Patrol", &LoadPatrolSystem);
    Spoon::SystemRegistry::Get().RegisterLoader("Movement", &LoadMovementSystem);
    Spoon::SystemRegistry::Get().RegisterLoader("Player Controller", &LoadPlayerController);
}