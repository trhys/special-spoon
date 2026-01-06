#pragma once

#include "Core/Core.h"
#include "Core/ComponentArray.h"

#include "ECS/ECS.h"
#include "ECS/Entity.h"

#include "Utils/Macros.h"

#include <memory>

namespace Spoon
{
    class EntityManager
    {
    public:
        EntityManager() { LoadDefaultArrays(); }

    public:
        // ===========================================
        // Entity management methods
        // ===========================================

        UUID CreateEntity(std::string displayName = "");    // Creates a new entity and returns its UUID (optional display name for editor)
        UUID GenerateID();                                  // Generates a new unique UUID
        void LoadEntity(UUID id, std::string name);         // Loads an entity with a specific UUID (for deserialization)
        void KillEntity(UUID id);                           // Removes an entity and recycles its UUID
        void ClearEntities();                               // Removes all entities

        // todo: #41 one of these is redundant but im not sure which one
        const std::unordered_map<UUID, std::string>& GetAllEntities() { return m_Entities; }
        std::unordered_map<UUID, std::string>& GetEntities() { return m_Entities; }
    
    public:
        // ===========================================
        // Component management methods
        // ===========================================

        // Template specialization alternative for adding components
        template<typename COMP, typename... Args>
        void MakeComponent(UUID id, Args&&... args);

        // Template specialization alternative for removing components
        template<typename COMP>
        void KillComponent(UUID id);

        // Non-template version for removing components
        void KillComponent(std::string type, UUID id);

        // Load a component array for a specific component type
        template<typename COMP>
        void LoadArray(std::string displayName);

        // Get a reference to a specific component array
        template<typename COMP>
        ComponentArray<COMP>& GetArray();

        // Get a specific component of an entity
        template<typename COMP>
        COMP& GetComponent(UUID id);

        // Get all entities that have a specific component
        template<typename COMP>
        std::vector<UUID> GetAllEntitiesWithComponent();

        // Get all components of a specific entity
        std::vector<Component*> GetAllComponentsOfEntity(UUID id);

        // Get all component arrays
        const std::unordered_map<std::string, std::unique_ptr<IComponentArray>>& GetAllArrays() { return m_Arrays; }

        // Get all component creators
        const std::unordered_map<std::string, std::function<void(UUID)>>& GetCreators() { return m_CompCreators; }

    public:
        // ===========================================
        // Misc Methods
        // ===========================================

        // Buffer an action for an entity
        void PushAction(UUID entityId, std::string action);

        // Clear the action buffer
        void ClearActionsBuffer();

        // Get the action buffer
        std::unordered_map<UUID, std::string>& GetActionsBuffer() { return m_ActionsBuffer; }

        // Clear all component arrays
        void ClearArrays();

        // Clear all entities and component arrays and actions
        void Shutdown();

        // Raypick to get entities under a specific world coordinate
        std::vector<UUID> RayPick(sf::Vector2f worldMouse);

    private:
        std::uint64_t m_IdCounter = 0;
        std::vector<UUID> m_RecycledIds;

        std::unordered_map<UUID, std::string> m_Entities;                           // Maps UUID to a debug name
        std::unordered_map<std::string, std::unique_ptr<IComponentArray>> m_Arrays; // Maps type name to array object
        std::unordered_map<std::string, std::function<void(UUID)>> m_CompCreators;  // Stores methods for adding new components
        std::unordered_map<UUID, std::string> m_ActionsBuffer;                      // Maps UUID to action string

    private:
        // Load default component arrays at startup
        void LoadDefaultArrays();
    };
}

#include "EntityManager.inl"