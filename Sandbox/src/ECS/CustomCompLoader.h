#include "Spoon.h"

#include "Component/PatrolComp.h"

void LoadPatrolComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
{
    float pointA = comp["PointA"].get<float>();
    float pointB = comp["PointB"].get<float>();
    if(comp.contains("IdleTime"))
    {
        float idleTime = comp["IdleTime"].get<float>();
        manager.MakeComponent<PatrolComp>(id, pointA, pointB, idleTime);
    }
    else { manager.MakeComponent<PatrolComp>(id, pointA, pointB); }
}

void RegisterCustomLoaders()
{
    SS_DEBUG_LOG("Registering Patrol Component Loader")
    Spoon::ComponentLoaders::RegisterCompLoader("Patrol", &LoadPatrolComponent);
}