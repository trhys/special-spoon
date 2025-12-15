#include "Entity.h"
#include "EntityManager.h"

template<typename COMP, typename... Args>
void Entity::AddComponent(Args&&... args)
{
    p_EM->MakeComponent<COMP>(m_ID, std::forward<Args>(args...));
}

template<typename COMP>
void Entity::DropComponent()
{
    p_EM->KillComponent<COMP>(m_ID);
}