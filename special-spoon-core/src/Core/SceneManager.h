#pragma once

#include "Core.h"
#include "SFML/Graphics/Rect.hpp"
#include <unordered_map>
#include <string>

namespace Spoon
{
    class EntityManager;
    class SystemManager;
    
    struct SceneData
    {
        std::string ID;
        std::string ResourceFiles;
        std::string DataFiles;
    };

    class SPOON_API SceneManager
    {
    public:
        SceneManager() {}
        ~SceneManager() {}

        void LoadManifest(std::string manifestPath);
        void LoadScene(std::string id, EntityManager& entityManager, SystemManager& systemManager);

        void UnloadScene(EntityManager& entityManager, SystemManager& systemManager);
        void Transition(std::string id, EntityManager& eManager, SystemManager& sManager);

    private:
        std::unordered_map<std::string, SceneData> m_SceneManifest;
    };
}