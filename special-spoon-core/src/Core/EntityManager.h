#pragma once

#include "ECS/Entity.h"
#include "ComponentArray.h"
#include <memory>

namespace Spoon
{
    class EntityManager
    {
    public:
        EntityManager() {}
        ~EntityManager() {}

        Entity CreateEntity()
        {
            Entity entity = Entity(GenerateID(), this);
            return entity;
        }

        UUID GenerateID()
        {
            if(!m_RecycledIds.empty())
            {
                std::uint64_t id = m_RecycledIds.back();
                m_RecycledIds.pop_back();
                return UUID(id);
            }
            else { return UUID(m_IdCounter++); }
        }

        template<typename COMP, typename... Args>
        void MakeComponent(UUID id, Args&&... args)
        {
            const char* type = typeid(COMP).name();
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[type].get());

            COMP newcomp = COMP(std::forward<Args>(args)...);
            array->AddComponent(id, newcomp);
        }

        template<typename COMP>
        void KillComponent(UUID id)
        {
            const char* type = typeid(COMP).name();
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[type].get());
            array->RemoveComponent(id);
        }

        template<typename T>
        void LoadArray()
        {
            const char* name = typeid(T).name();
            m_Arrays[name] = std::make_unique<ComponentArray<T>>();
        }

        template<typename T>
        ComponentArray<T>& GetArray()
        {
            const char* name = typeid(T).name();
            ComponentArray<T>* array = static_cast<ComponentArray<T>*>(m_Arrays[name].get());
            return *array;
        }

        template<typename COMP>
        std::vector<UUID> GetAllEntitiesWithComponent()
        {
            std::vector<UUID> entities;

            const char* name = typeid(COMP).name();
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[name].get());
            
            entities = array->GetAllEntities();

            return entities;
        }

        template<typename COMP>
        COMP* GetComponent(UUID id)
        {
            const char* name = typeid(COMP).name();
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[name].get());

            size_t index = array->m_IdToIndex[id];
            return &array->m_Components[index];
        }

    private:
        std::uint64_t m_IdCounter = 0;
        std::vector<std::uint64_t> m_RecycledIds;

        std::unordered_map<const char*, std::unique_ptr<IComponentArray>> m_Arrays;
    };
}