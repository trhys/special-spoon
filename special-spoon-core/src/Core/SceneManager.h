#pragma once

#include "Core.h"
#include "SFML/Graphics/Rect.hpp"
#include <unordered_map>

namespace Spoon
{
    class EntityManager;
    class SystemManager;
    
    struct SceneData
    {
        std::string ID;
        std::string ResourceFiles;
        std::string DataFiles;
        sf::Vector2f Bounds;
    };

    class SPOON_API SceneManager
    {
    public:
        SceneManager() {}
        ~SceneManager() {}

        void LoadManifest(std::string manifestPath);
        void LoadScene(std::string id, EntityManager& entityManager, SystemManager& systemManager);

    private:
        std::unordered_map<std::string, SceneData> m_SceneManifest;
    };
}