#pragma once

#include "System.h"

namespace Spoon
{
    class SPOON_API SystemManager
    {
    public:
        SystemManager() {}
        ~SystemManager() {}

        void AddSystem(ISystem* system)
        {
            m_Systems.push_back(system);
        }

        void ClearSystems()
        {
            m_Systems.clear();
        }

        void UpdateSystems(sf::Time tick, EntityManager& manager)
        {
            for(auto& system : m_Systems)
            {
                system->Update(tick, manager);
            }
        }

    private:
        std::vector<ISystem*> m_Systems;
    };
}