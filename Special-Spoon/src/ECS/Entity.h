#pragma once

#include "Core/Core.h"
#include "ECS/UUID.h"
#include "ECS/Components/Component.h"

namespace Spoon
{   
    class EntityManager;

    class SPOON_API Entity
    {
    public:
        Entity() {}
        Entity(UUID id, EntityManager* em) : m_ID(id), p_EM(em) {}
        ~Entity() = default;
        
        template<typename COMP, typename... Args>
        void AddComponent(Args&&... args);

        template<typename COMP>
        void DropComponent();

        UUID GetID() const { return m_ID; }

    private:
        EntityManager* p_EM;
        UUID m_ID;
    };
}