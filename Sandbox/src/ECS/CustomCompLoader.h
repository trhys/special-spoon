#include "Spoon.h"

#include "Component/PatrolComp.h"
#include "Component/MovementComp.h"
#include "Component/PlayerComp.h"

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

void LoadMovementComp(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
{
    float speed = comp["Speed"].get<float>();
    manager.MakeComponent<MovementComp>(id, speed);
}

void LoadPlayerComp(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
{
    manager.MakeComponent<PlayerComp>(id);
}

void RegisterCustomLoaders()
{
    Spoon::ComponentLoaders::RegisterCompLoader("PatrolComp", &LoadPatrolComponent);
    Spoon::ComponentLoaders::RegisterCompLoader("Movement", &LoadMovementComp);
    Spoon::ComponentLoaders::RegisterCompLoader("PlayerComp", &LoadPlayerComp);
}