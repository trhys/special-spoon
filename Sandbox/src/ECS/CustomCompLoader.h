#include "Spoon.h"

#include "Component/PatrolComp.h"
#include "Component/MovementComp.h"
#include "Component/PlayerComp.h"

void LoadPatrolComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)
{
    std::vector<sf::Vector2f> patrolPoints;
    if (comp.contains("PatrolPoints"))
    {
        for (const auto& point : comp["PatrolPoints"])
        {
            float x = point["x"].get<float>();
            float y = point["y"].get<float>();
            patrolPoints.emplace_back(x, y);
        }
    }
    if (comp.contains("IdleTime"))
    {
        float idleTime = comp["IdleTime"].get<float>();
        manager.MakeComponent<PatrolComp>(id, patrolPoints, idleTime);
    }
    else { manager.MakeComponent<PatrolComp>(id, patrolPoints, 0.0f); }

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
    Spoon::ComponentLoaders::RegisterCompLoader(PatrolComp::Name, &LoadPatrolComponent);
    Spoon::ComponentLoaders::RegisterCompLoader(MovementComp::Name, &LoadMovementComp);
    Spoon::ComponentLoaders::RegisterCompLoader(PlayerComp::Name, &LoadPlayerComp);
}