#pragma once

#include "Core.h"
#include "ECS/Component.h"

#include <cstdint>

namespace Spoon
{   
    class EntityManager;

    struct UUID
    {
        UUID(std::uint64_t id) : ID(id) {}
        std::uint64_t ID;
    };

    class SPOON_API Entity
    {
    public:
        Entity(UUID id, EntityManager* em) : m_ID(id), p_EM(em) {}
        ~Entity();
        
        template<typename COMP, typename... Args>
        void AddComponent(Args&&... args)
        {
            p_EM->MakeComponent<COMP>(m_ID, std::forward<Args>(args...));
        }

        template<typename COMP>
        void DropComponent()
        {
            p_EM->KillComponent<COMP>(m_ID);
        }

    private:
        EntityManager* p_EM;
        UUID m_ID;
    };
}