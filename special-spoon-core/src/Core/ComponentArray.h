#pragma once

#include <unordered_map>

namespace Spoon
{
    struct UUID;
    class IComponentArray {};

    template<typename COMP>
    struct ComponentArray : public IComponentArray
    {
        std::vector<COMP> m_Components;
        std::unordered_map<UUID, size_t> m_IdToIndex;
        std::unordered_map<size_t, UUID> m_IndexToId;

        void AddComponent(UUID id, COMP comp)
        {
            size_t index = m_Components.size();
            m_IdToIndex[id] = index;
            m_IndexToId[index] = id;
            m_Components.push_back(comp);
        }

        void RemoveComponent(UUID id)
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
    };
}