#include "EntityManager.h"

namespace Spoon
{
    // Load default component arrays (done at startup)
    void EntityManager::LoadDefaultArrays()
    {
        SS_DEBUG_LOG("Loading default component arrays")
        LoadArray<TransformComp>(TransformComp::Name);
        LoadArray<InputComp>(InputComp::Name);
        LoadArray<StatusComp>(StatusComp::Name);
        LoadArray<SpriteComp>(SpriteComp::Name);
        LoadArray<TextComp>(TextComp::Name);
        LoadArray<ColorComp>(ColorComp::Name);
        LoadArray<PhysicsComp>(PhysicsComp::Name);
        LoadArray<BlinkComp>(BlinkComp::Name);
        LoadArray<FadeComp>(FadeComp::Name);
        LoadArray<AnimationComp>(AnimationComp::Name);
        LoadArray<StateActionComp>(StateActionComp::Name);
        LoadArray<RenderLayer>(RenderLayer::Name);
    }

    // ===========================================
    // Entity Creation/Management Methods
    // ===========================================
    UUID EntityManager::CreateEntity(std::string displayName)
    {
        UUID id = GenerateID();
        Entity entity = Entity(id, this);
        m_Entities[id] = displayName;
        return id;
    }

    UUID EntityManager::GenerateID()
    {
        if(!m_RecycledIds.empty())
        {
            UUID id = m_RecycledIds.back();
            m_RecycledIds.pop_back();
            return id;
        }
        else { return UUID(m_IdCounter++); }
    }

    void EntityManager::LoadEntity(UUID id, std::string name)
    {
        auto found = m_Entities.find(id);
        if (found == m_Entities.end())
        {
            m_Entities[id] = name;
            m_IdCounter++;
        }
        else CreateEntity(name);
    }

    void EntityManager::KillEntity(UUID id)
    {
        m_Entities.erase(id);
        m_RecycledIds.push_back(id);
    }

    void EntityManager::ClearEntities()
    {
        m_Entities.clear();
    }

    // ===========================================
    // Component Management Methods
    // ===========================================
    void EntityManager::KillComponent(std::string type, UUID id)
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

    std::vector<Component*> EntityManager::GetAllComponentsOfEntity(UUID id)
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
    
    // ===========================================
    // Misc Methods
    // ===========================================
    void EntityManager::PushAction(UUID entityId, std::string action)
    {
        SS_DEBUG_LOG("[ENTITY MANAGER] Buffering action: " + action)
        m_ActionsBuffer[entityId] = action;
    }

    void EntityManager::ClearActionsBuffer()
    {
        m_ActionsBuffer.clear();
    }

    void EntityManager::ClearArrays()
    {
        for(auto& [type, array] : m_Arrays)
        {
            array->Clear();
        }
    }

    void EntityManager::Shutdown()
    {
        m_Arrays.clear();
        m_Entities.clear();
        m_ActionsBuffer.clear();
        m_RecycledIds.clear();
        m_IdCounter = 0;
    }

    std::vector<UUID> EntityManager::RayPick(sf::Vector2f worldMouse)
    {
        std::vector<UUID> pickedEntities;

        auto& transArray = GetArray<TransformComp>(TransformComp::Name);
        auto& spriteArray = GetArray<SpriteComp>(SpriteComp::Name);
        auto& textArray = GetArray<TextComp>(TextComp::Name);

        for (size_t index = 0; index < transArray.m_Components.size(); index++)
        {
            TransformComp& transform = transArray.m_Components[index];
            UUID id = transArray.m_IndexToId[index];
        
            if (!spriteArray.HasEntity(id))
            {
                if (textArray.HasEntity(id))
                {
                    TextComp& textComp = GetComponent<TextComp>(id, TextComp::Name);
                    sf::FloatRect bounds = textComp.m_Text.getGlobalBounds();
                    if (bounds.contains(worldMouse))
                    {
                        pickedEntities.push_back(id);
                    }
                }
                continue;
            }

            SpriteComp& spriteComp = GetComponent<SpriteComp>(id, SpriteComp::Name);
            sf::FloatRect bounds = spriteComp.m_Sprite.getGlobalBounds();
            if (bounds.contains(worldMouse))
            {
                pickedEntities.push_back(id);
            }
        }

        return pickedEntities;
    }
}