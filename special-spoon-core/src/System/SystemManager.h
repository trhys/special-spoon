#pragma once

#include "System.h"
#include "SystemLoaders.h"
#include <memory>

namespace Spoon
{
    class SPOON_API SystemManager
    {
    public:
        SystemManager() {}
        ~SystemManager() {}

        SystemManager(const SystemManager&) = delete;
        SystemManager& operator=(const SystemManager&) = delete;

        void AddSystem(const json& systemData)
        {
            std::string type = systemData["Type"].get<std::string>();
            auto found = SystemLoaders::s_SysLoaders.find(type);
            if(found != SystemLoaders::s_SysLoaders.end())
            {
                m_Systems.emplace_back(found->second(systemData));
            }
            else
            {
                throw std::runtime_error("No system loader registered for type: " + type);
            }
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
        std::vector<std::unique_ptr<ISystem>> m_Systems;
    };
}