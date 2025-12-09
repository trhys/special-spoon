#include "Spoon.h"

class PatrolSystem : public Spoon::ISystem
{
public:
    PatrolSystem() {}
    ~PatrolSystem() {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& patrolArray = manager.GetAllEntitiesWithComponent<PatrolComp>();
        for(auto& entity : patrolArray)
        {
            // PatrolComp& patrol = patrolArray.m_Components[patrolArray.m_IdToIndex[entity]];
            Spoon::TransformComp& transform = manager.GetComponent<Spoon::TransformComp>(entity);

            // TODO
        }
    }
};