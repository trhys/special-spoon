#include "Spoon.h"

#include "System/PatrolSystem.h"

std::unique_ptr<Spoon::ISystem> LoadPatrolSystem(const nlohmann::json& systemData)
{
    return std::make_unique<PatrolSystem>();
}

void RegisterCustomSystems()
{
    Spoon::SystemLoaders::RegisterSysLoader("Patrol", &LoadPatrolSystem);
}