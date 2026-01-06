#include "Spoon.h"

#include "ECS/Component/PatrolComp.h"
#include "ECS/Component/MovementComp.h"

class PatrolSystem : public Spoon::ISystem
{
public:
    PatrolSystem() : Spoon::ISystem::ISystem("Patrol") {}
    ~PatrolSystem() {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& patrolArray = manager.GetArray<PatrolComp>(PatrolComp::Name);
        auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
        auto& transformArray = manager.GetArray<Spoon::TransformComp>(Spoon::TransformComp::Name);
        for(size_t in = 0; in < patrolArray.m_Components.size(); in++)
        {
            PatrolComp& patrol = patrolArray.m_Components[in];
            Spoon::UUID entity = patrolArray.m_IndexToId[in];
            
            if(movementArray.m_IdToIndex.count(entity) &&
                transformArray.m_IdToIndex.count(entity))
            {
                MovementComp& moveComp = manager.GetComponent<MovementComp>(entity);
                Spoon::TransformComp& transComp = manager.GetComponent<Spoon::TransformComp>(entity);

                sf::Vector2f target;
                target = patrol.m_Destination - transComp.GetPosition();
                float normal = std::sqrt(target.x * target.x + target.y * target.y);

                if(normal > 0)
                {
                    target /= normal;

                    moveComp.m_Velocity = target * moveComp.m_Speed;
                }
                else
                {
                    moveComp.m_Velocity = {0.0, 0.0};
                    patrol.m_CurrentIdle += tick.asSeconds();
                    if(patrol.m_CurrentIdle > patrol.m_IdleTime)
                    {
                        patrol.NextPoint();
                    }
                }
            }
            else
            {
                SS_DEBUG_LOG("[PATROL SYSTEM] Entity has no movement and/or transform!");
            }
        }
    }
};