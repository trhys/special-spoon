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
    Spoon::SystemLoaders::RegisterSysLoader("Patrol", &LoadPatrolSystem);
    Spoon::SystemLoaders::RegisterSysLoader("Movement", &LoadMovementSystem);
    Spoon::SystemLoaders::RegisterSysLoader("Player Controller", &LoadPlayerController);
}