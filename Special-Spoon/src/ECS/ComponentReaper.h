#pragma once

#include "UUID.h"
#include <vector>

namespace Spoon
{
    class EntityManager;

    class ComponentReaper
    {
        public:
            void AddId(UUID id)
            {
                m_IdCache.push_back(id);
            }
            
            void ReapComponents(EntityManager* manager);

        private:
            std::vector<UUID> m_IdCache;
    };
}