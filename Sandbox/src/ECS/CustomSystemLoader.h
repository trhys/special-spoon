#include "Spoon.h"

/*#include "System/PatrolSystem.h"*/
#include "System/MovementSystem.h"

/*std::unique_ptr<Spoon::ISystem> LoadPatrolSystem(const nlohmann::json* systemData)*/
/*{*/
/*    return std::make_unique<PatrolSystem>();*/
/*}*/

std::unique_ptr<Spoon::ISystem> LoadMovementSystem(const nlohmann::json* systemData)
{
    return std::make_unique<MovementSystem>();
}

void RegisterCustomSystems()
{
    /*Spoon::SystemRegistry::Get().RegisterLoader("Patrol", &LoadPatrolSystem);*/
    Spoon::SystemRegistry::Get().RegisterLoader("Movement", &LoadMovementSystem);
}
