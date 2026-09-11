#pragma once

#include "System.h"
#include "StateSystem.h"
#include "InputSystem.h"

#include "Core/Registers/System/SystemRegistry.h"

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

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
                auto& loaderMap = SystemRegistry::Get().GetLoaders();
                auto found = loaderMap.find(type);
                if(found != loaderMap.end())
                {
                    m_Systems.emplace_back(found->second(systemData));
                    m_SystemOrderDirty = true;
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
            auto& loaderMap = SystemRegistry::Get().GetLoaders();
            auto found = loaderMap.find(type);
            if(found != loaderMap.end())
            {
                m_Systems.emplace_back(found->second(systemData));
                m_SystemOrderDirty = true;
            }
            else
            {
                throw std::runtime_error("No system loader registered for type: " + type);
            }
        }

        void ClearSystems()
        {
            m_Systems.clear();
            m_SystemOrderDirty = true;
        }

        void UpdateSystems(sf::Time tick, EntityManager& manager)
        {
            if (m_SystemOrderDirty)
            {
                EnforceKnownExecutionOrder();
                m_SystemOrderDirty = false;
            }
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

        void InitializeStateSystem()
        {
            m_StateSystem = std::make_unique<StateSystem>();
        }

        StateSystem* GetStateSystem() { return m_StateSystem.get(); }
        
        std::vector<std::unique_ptr<ISystem>>& GetSystems()
        {
            m_SystemOrderDirty = true;
            return m_Systems;
        }

    private:
        void EnforceKnownExecutionOrder()
        {
            if (m_Systems.size() < 2)
                return;

            std::unordered_map<std::string, size_t> nameToIndex;
            for (size_t index = 0; index < m_Systems.size(); index++)
            {
                nameToIndex[m_Systems[index]->GetDisplayName()] = index;
            }

            std::vector<std::vector<size_t>> edges(m_Systems.size());
            std::vector<size_t> indegree(m_Systems.size(), 0);
            constexpr std::array<std::pair<const char*, const char*>, 3> dependencies = {
                std::pair{ "Movement", "Physics" },
                std::pair{ "Physics", "Animation" },
                std::pair{ "Movement", "Animation" }
            };

            for (const auto& [before, after] : dependencies)
            {
                auto beforeIt = nameToIndex.find(before);
                auto afterIt = nameToIndex.find(after);
                if (beforeIt == nameToIndex.end() || afterIt == nameToIndex.end())
                    continue;

                edges[beforeIt->second].push_back(afterIt->second);
                indegree[afterIt->second]++;
            }

            std::vector<size_t> resolvedOrder;
            resolvedOrder.reserve(m_Systems.size());
            std::vector<bool> used(m_Systems.size(), false);

            for (size_t count = 0; count < m_Systems.size(); count++)
            {
                size_t nextIndex = m_Systems.size();
                for (size_t index = 0; index < m_Systems.size(); index++)
                {
                    if (!used[index] && indegree[index] == 0)
                    {
                        nextIndex = index;
                        break;
                    }
                }

                if (nextIndex == m_Systems.size())
                    return;

                used[nextIndex] = true;
                resolvedOrder.push_back(nextIndex);

                for (size_t dependent : edges[nextIndex])
                {
                    indegree[dependent]--;
                }
            }

            bool changed = false;
            for (size_t index = 0; index < resolvedOrder.size(); index++)
            {
                if (resolvedOrder[index] != index)
                {
                    changed = true;
                    break;
                }
            }

            if (!changed)
                return;

            std::vector<std::unique_ptr<ISystem>> orderedSystems;
            orderedSystems.reserve(m_Systems.size());
            for (size_t index : resolvedOrder)
            {
                orderedSystems.push_back(std::move(m_Systems[index]));
            }

            m_Systems = std::move(orderedSystems);
        }

        std::vector<std::unique_ptr<ISystem>> m_Systems;
        std::unique_ptr<StateSystem> m_StateSystem = nullptr;
        bool m_SystemOrderDirty = true;
    };
}