#pragma once

#include "ECS/Entity.h"
#include <unordered_map>
#include <vector>

namespace Spoon
{
    struct UUID;
    class IComponentArray 
    {
    public:
        IComponentArray(std::string id) : m_DisplayName(id) {}
        virtual std::string GetDisplayName() { return m_DisplayName; }
        virtual void Clear() {}
        virtual bool HasEntity(UUID id) = 0;
        virtual void RemoveComponent(UUID id) {}
        virtual Component* GetRawComp(UUID id) = 0;

        std::string m_DisplayName;
    };

    template<typename COMP>
    struct ComponentArray : public IComponentArray
    {
        std::vector<COMP> m_Components;
        std::unordered_map<UUID, size_t> m_IdToIndex;
        std::unordered_map<size_t, UUID> m_IndexToId;
        
        ComponentArray(std::string id) : IComponentArray::IComponentArray(id) {}

        void AddComponent(UUID id, COMP comp)
        {
            size_t index = m_Components.size();
            m_IdToIndex[id] = index;
            m_IndexToId[index] = id;
            m_Components.push_back(comp);
        }

        void RemoveComponent(UUID id) override
        {
            size_t deleteme = m_IdToIndex[id];
            size_t last = m_Components.size() - 1;
            UUID lastID = m_IndexToId[last];

            m_Components[deleteme] = m_Components[last];
            m_Components.pop_back();

            m_IdToIndex[lastID] = deleteme;
            m_IndexToId[deleteme] = lastID;

            m_IdToIndex.erase(id);
            m_IndexToId.erase(last);
        }

        std::vector<UUID> GetAllEntities()
        {
            std::vector<UUID> entities;
            for(auto& [id, index] : m_IdToIndex)
            {
                entities.push_back(id);
            }
            return entities;
        }

        void Clear() override
        {
            m_Components.clear();
            m_IdToIndex.clear();
            m_IndexToId.clear();
        }

        bool HasEntity(UUID id) override
        {
            if(m_IdToIndex.count(id)) return true;
            else return false;
        }

        Component* GetRawComp(UUID id) override
        {
            auto index = m_IdToIndex[id];
            Component* comp = &m_Components[index];
            return comp;
        }
    };
}