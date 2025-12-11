#include "Spoon.h"

#include "Component/PatrolComp.h"

void LoadPatrolComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
{

    sf::Vector2f pointA(comp["PointA"]["x"].get<float>(), comp["PointA"]["y"].get<float>());
    sf::Vector2f pointB(comp["PointB"]["x"].get<float>(), comp["PointB"]["y"].get<float>());
    if (comp.contains("IdleTime"))
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