#pragma once

#include "Core.h"
#include "Utils/Macros.h"
#include "ECS/ECS.h"
#include "ECS/Entity.h"
#include "ComponentArray.h"

#include <memory>

namespace Spoon
{
    class EntityManager
    {
    public:
        EntityManager() { LoadDefaultArrays(); }
        ~EntityManager() {}

        Entity CreateEntity(std::string debugName = "")
        {
            UUID id = GenerateID();
            Entity entity = Entity(id, this);
            m_Entities[id] = debugName;
            return entity;
        }

        void KillEntity(UUID id)
        {
            m_Entities.erase(id);
            m_RecycledIds.push_back(id);
        }

        UUID GenerateID()
        {
            if(!m_RecycledIds.empty())
            {
                UUID id = m_RecycledIds.back();
                m_RecycledIds.pop_back();
                return id;
            }
            else { return UUID(m_IdCounter++); }
        }

        const std::unordered_map<UUID, std::string>& GetAllEntities()
        {
            return m_Entities;
        }

        template<typename COMP, typename... Args>
        void MakeComponent(UUID id, Args&&... args)
        {
            std::string type = typeid(COMP).name();
            auto found = m_Arrays.find(type);
            if (found == m_Arrays.end())
            {
                std::string errorMsg = "ERROR: Attempted to MakeComponent<";
                errorMsg += type;
                errorMsg += "> but no ComponentArray storage was found. Did you forget to call LoadArray<COMP>() in startup?";
                throw std::runtime_error(errorMsg);
            }
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[type].get());

            COMP newcomp = COMP(std::forward<Args>(args)...);
            array->AddComponent(id, newcomp);
        }

        template<typename COMP>
        void KillComponent(UUID id)
        {
            std::string type = typeid(COMP).name();
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[type].get());
            array->RemoveComponent(id);
        }

        void KillComponent(std::string type, UUID id)
        {
            auto found = m_Arrays.find(type);
            if(found != m_Arrays.end())
            {
                found->second->RemoveComponent(id);
            }
            else
            {
                throw std::runtime_error("Type or entity ID not found");
            }
        }

        template<typename COMP>
        void LoadArray(std::string displayName)
        {
            std::string name = typeid(COMP).name();
            SS_DEBUG_LOG("[ENTITY MANAGER] Loading component array: " + name)
            m_Arrays[name] = std::make_unique<ComponentArray<COMP>>(displayName);
            m_CompCreators[name] = [this](UUID id) { this->MakeComponent<COMP>(id); };
        }

        template<typename COMP>
        ComponentArray<COMP>& GetArray()
        {
            std::string name = typeid(COMP).name();
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[name].get());
            return *array;
        }

        const std::unordered_map<std::string, std::unique_ptr<IComponentArray>>& GetAllArrays()
        {
            return m_Arrays;
        }

        const std::unordered_map<std::string, std::function<void(UUID)>>& GetCreators()
        {
            return m_CompCreators;
        }

        template<typename COMP>
        std::vector<UUID> GetAllEntitiesWithComponent()
        {
            std::vector<UUID> entities;

            std::string name = typeid(COMP).name();
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[name].get());
            
            entities = array->GetAllEntities();

            return entities;
        }

        template<typename COMP>
        COMP& GetComponent(UUID id)
        {
            std::string name = typeid(COMP).name();
            if(m_Arrays.find(name) == m_Arrays.end())
            {
                throw std::runtime_error("Component array does not exist for type: " + name);
            }
            ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[name].get());

            if(array->m_IdToIndex.find(id) == array->m_IdToIndex.end())
            {
                throw std::runtime_error("Entity does not have component type: " + name);
            }
            size_t index = array->m_IdToIndex[id];
            return array->m_Components[index];
        }

        std::vector<Component*> GetAllComponentsOfEntity(UUID id)
        {
            std::vector<Component*> allComps;

            for(auto& [type, array] : m_Arrays)
            {
                if(array->HasEntity(id))
                {
                    Component* comp = array->GetRawComp(id);
                    allComps.push_back(comp);
                }
            }
            return allComps;
        }

        void PushAction(UUID entityId, std::string action)
        {
            SS_DEBUG_LOG("[ENTITY MANAGER] Buffering action: " + action)
            m_ActionsBuffer[entityId] = action;
        }

        void ClearActionsBuffer()
        {
            m_ActionsBuffer.clear();
        }

        std::unordered_map<UUID, std::string>& GetActionsBuffer()
        {
            return m_ActionsBuffer;
        }

        void ClearArrays()
        {
            for(auto& [type, array] : m_Arrays)
            {
                array->Clear();
            }
        }

        void ClearEntities()
        {
            m_Entities.clear();
        }

        std::unordered_map<UUID, std::string>& GetEntities() { return m_Entities; }

    private:
        std::uint64_t m_IdCounter = 0;
        std::vector<UUID> m_RecycledIds;

        std::unordered_map<UUID, std::string> m_Entities;                           // Maps UUID to a debug name
        std::unordered_map<std::string, std::unique_ptr<IComponentArray>> m_Arrays; // Maps type name to array object
        std::unordered_map<std::string, std::function<void(UUID)>> m_CompCreators;  // Stores methods for adding new components
        std::unordered_map<UUID, std::string> m_ActionsBuffer;                      // Maps UUID to action string


        void LoadDefaultArrays()
        {
            SS_DEBUG_LOG("Loading default component arrays")
            LoadArray<TransformComp>("Transform");
            LoadArray<InputComp>("Input");
            LoadArray<StatusComp>("Status");
            LoadArray<SpriteComp>("Sprite");
            LoadArray<TextComp>("Text");
            LoadArray<ColorComp>("Color");
            LoadArray<PhysicsComp>("Physics");
            LoadArray<BlinkComp>("Blink");
            LoadArray<FadeComp>("Fade");
            LoadArray<AnimationComp>("Animation");
            LoadArray<StateActionComp>("StateAction");
            LoadArray<RenderLayer>("Layer");
        }
    };
}