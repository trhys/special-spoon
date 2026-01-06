namespace Spoon
{
    template<typename COMP, typename... Args>
    void EntityManager::MakeComponent(UUID id, Args&&... args)
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
    void EntityManager::KillComponent(UUID id)
    {
        std::string type = typeid(COMP).name();
        ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[type].get());
        array->RemoveComponent(id);
    }

    template<typename COMP>
    void EntityManager::LoadArray(std::string displayName)
    {
        std::string name = typeid(COMP).name();
        SS_DEBUG_LOG("[ENTITY MANAGER] Loading component array: " + name)
        m_Arrays[name] = std::make_unique<ComponentArray<COMP>>(displayName);
        m_CompCreators[displayName] = [this](UUID id) { this->MakeComponent<COMP>(id); };
    }

    template<typename COMP>
    ComponentArray<COMP>& EntityManager::GetArray()
    {
        std::string name = typeid(COMP).name();
        ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[name].get());
        return *array;
    }

    template<typename COMP>
    std::vector<UUID> EntityManager::GetAllEntitiesWithComponent()
    {
        std::vector<UUID> entities;

        std::string name = typeid(COMP).name();
        ComponentArray<COMP>* array = static_cast<ComponentArray<COMP>*>(m_Arrays[name].get());
        
        entities = array->GetAllEntities();

        return entities;
    }

    template<typename COMP>
    COMP& EntityManager::GetComponent(UUID id)
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
}