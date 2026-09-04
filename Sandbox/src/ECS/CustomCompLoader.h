#include "Spoon.h"

/*#include "Component/PatrolComp.h"*/

/*void LoadPatrolComponent(Spoon::EntityManager& manager, Spoon::UUID id, const nlohmann::json& comp)*/
/*{*/
/*    std::vector<sf::Vector2f> patrolPoints;*/
/*    if (comp.contains("PatrolPoints"))*/
/*    {*/
/*        for (const auto& point : comp["PatrolPoints"])*/
/*        {*/
/*            float x = point["x"].get<float>();*/
/*            float y = point["y"].get<float>();*/
/*            patrolPoints.emplace_back(x, y);*/
/*        }*/
/*    }*/
/*    if (comp.contains("IdleTime"))*/
/*    {*/
/*        float idleTime = comp["IdleTime"].get<float>();*/
/*        manager.MakeComponent<PatrolComp>(id, PatrolComp::Name, patrolPoints, idleTime);*/
/*    }*/
/*    else { manager.MakeComponent<PatrolComp>(id, PatrolComp::Name, patrolPoints, 0.0f); }*/
/**/
/*}*/

void RegisterCustomLoaders()
{
    /*Spoon::ComponentRegistry::Get().RegisterLoader(PatrolComp::Name, &LoadPatrolComponent);*/
}
