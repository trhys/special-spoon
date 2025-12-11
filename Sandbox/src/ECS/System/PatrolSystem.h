#include "Spoon.h"

class PatrolSystem : public Spoon::ISystem
{
public:
    PatrolSystem() {}
    ~PatrolSystem() {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& patrolArray = manager.GetArray<PatrolComp>();
        for(size_t in = 0; in < patrolArray.m_Components.size(); in++)
        {
            PatrolComp& patrol = patrolArray.m_Components[in];
            Spoon::UUID entity = patrolArray.m_IndexToId[in];
            

            // TODO
        }
    }
};