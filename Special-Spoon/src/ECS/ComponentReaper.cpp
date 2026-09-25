#include "ComponentReaper.h"
#include "Core/EntityManager/EntityManager.h"

namespace Spoon
{
    void ComponentReaper::ReapComponents(EntityManager* manager)
    {
        for (UUID id : m_IdCache)
        {
            manager->KillEntity(id);
        }
        m_IdCache.clear();
    }
}