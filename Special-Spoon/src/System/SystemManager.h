#pragma once

#include "System.h"
#include "StateSystem.h"
#include "InputSystem.h"

#include "Core/Registers/System/SystemRegistry.h"

#include <array>
#include <memory>
#include <queue>
#include <set>
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
                ScheduleSystems();
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

        void InitializeStateSystem(){ m_StateSystem = std::make_unique<StateSystem>(); }
        StateSystem* GetStateSystem() { return m_StateSystem.get(); }
        
        std::vector<std::unique_ptr<ISystem>>& GetSystems()
        { 
          m_SystemOrderDirty = true;
          return m_Systems; 
        }

    private:
        // does topological sorting of systems based on their dependencies
        void ScheduleSystems()
        {
            if (m_Systems.size() < 2)
                return;

            const std::size_t systemCount = m_Systems.size();

            // index system by name for quick lookup
            std::unordered_map<std::string, std::size_t> nameToIndex;
            nameToIndex.reserve(systemCount);

            for (std::size_t index = 0; index < systemCount; ++index)
            {
                const std::string& name =
                    m_Systems[index]->GetDisplayName();

                if (nameToIndex.count(name))
                {
                    throw std::runtime_error(
                        "Duplicate system name in scheduler: " + name
                    );
                }

                nameToIndex.emplace(name, index);
            }

            // build graph edges based on system dependencies
            std::set<std::pair<std::size_t, std::size_t>> uniqueEdges;

            auto addDependency = [&](std::size_t before,
                                    std::size_t after)
            {
                if (before == after)
                {
                    const std::string& name =
                        m_Systems[before]->GetDisplayName();

                    throw std::runtime_error(
                        "System declares a dependency on itself: " + name
                    );
                }

                uniqueEdges.emplace(before, after);
            };

            for (std::size_t index = 0; index < systemCount; ++index)
            {
                ISystem& system = *m_Systems[index];

                // get system dependencies
                for (const std::string& dependency : system.RunAfter())
                {
                    // get dependency's index in systems vector
                    auto dependencyIt = nameToIndex.find(dependency);

                    if (dependencyIt == nameToIndex.end())
                        continue;

                    addDependency(dependencyIt->second, index);
                }

                // find system dependents
                for (const std::string& dependent : system.RunBefore())
                {
                    // get index
                    auto dependentIt = nameToIndex.find(dependent);

                    if (dependentIt == nameToIndex.end())
                        continue;

                    addDependency(index, dependentIt->second);
                }
            }

            std::vector<std::vector<std::size_t>> outgoing(systemCount);
            std::vector<std::size_t> indegree(systemCount, 0);

            for (const auto& [before, after] : uniqueEdges)
            {
                outgoing[before].push_back(after);
                ++indegree[after];
            }

            // deterministic sort with index as tie-breaker
            std::priority_queue<
                std::size_t,
                std::vector<std::size_t>,
                std::greater<std::size_t>
            > ready;

            for (std::size_t index = 0; index < systemCount; ++index)
            {
                if (indegree[index] == 0)
                    ready.push(index);
            }

            std::vector<std::size_t> resolvedOrder;
            resolvedOrder.reserve(systemCount);

            while (!ready.empty())
            {
                const std::size_t current = ready.top();
                ready.pop();

                resolvedOrder.push_back(current);

                for (const std::size_t dependent : outgoing[current])
                {
                    if (--indegree[dependent] == 0)
                        ready.push(dependent);
                }
            }

            if (resolvedOrder.size() != systemCount)
            {
                std::string cycleMembers;

                for (std::size_t index = 0; index < systemCount; ++index)
                {
                    if (indegree[index] != 0)
                    {
                        if (!cycleMembers.empty())
                            cycleMembers += ", ";

                        cycleMembers +=
                            m_Systems[index]->GetDisplayName();
                    }
                }

                throw std::runtime_error(
                    "Cycle detected in system dependencies. "
                    "Systems involved: " + cycleMembers
                );
            }

            // dont rebuild unless order has changed
            bool changed = false;

            for (std::size_t index = 0; index < systemCount; ++index)
            {
                if (resolvedOrder[index] != index)
                {
                    changed = true;
                    break;
                }
            }

            if (!changed)
                return;

            // build scheduled vector from sorted graph
            std::vector<std::unique_ptr<ISystem>> orderedSystems;
            orderedSystems.reserve(systemCount);

            for (const std::size_t index : resolvedOrder)
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
