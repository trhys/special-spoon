#pragma once

#include "System.h"
#include "StateSystem.h"
#include "InputSystem.h"

#include "Core/Serialization/SystemLoaders.h"

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

        void AddSystem(const json* systemData)
        {
            if (systemData)
            {
                std::string type = (*systemData)["Type"].get<std::string>();
                auto& loaderMap = SystemLoaders::GetSysLoaders();
                auto found = loaderMap.find(type);
                if(found != loaderMap.end())
                {
                    m_Systems.emplace_back(found->second(systemData));
                }
                else
                {
                    throw std::runtime_error("No system loader registered for type: " + type);
                }
            }
            else
            {
                throw std::runtime_error("System data is nullptr! Only use AddSystem if deserializing!");
            }
        }

        void AddSystem(const std::string& type, const json* systemData = nullptr)
        {
            auto& loaderMap = SystemLoaders::GetSysLoaders();
            auto found = loaderMap.find(type);
            if(found != loaderMap.end())
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

        void UpdateState(sf::Time tick, EntityManager& manager)
        {
            if(m_StateSystem)
            {
                m_StateSystem->Update(tick, manager);
            }
        }

        void InitializeStateSystem(SceneManager& manager)
        {
            m_StateSystem = std::make_unique<StateSystem>(manager, *this);
        }

        std::unique_ptr<StateSystem>& GetStateSystem() { return m_StateSystem; }
        
        std::vector<std::unique_ptr<ISystem>>& GetSystems() { return m_Systems; }

    private:
        std::vector<std::unique_ptr<ISystem>> m_Systems;
        std::unique_ptr<StateSystem> m_StateSystem = nullptr;
    };
}