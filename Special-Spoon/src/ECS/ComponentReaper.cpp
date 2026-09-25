#include "ComponentReaper.h"
#include "Core/EntityManager/EntityManager.h"

namespace Spoon
{
    void ComponentReaper::ReapComponents(EntityManager* manager)
    {
        std::vector<UUID> idsToReap = m_IdCache;
        m_IdCache.clear();

        for (UUID id : idsToReap)
        {
            manager->KillEntity(id);
        }
    }
}