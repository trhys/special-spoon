#pragma once

#include "Core.h"
#include "SFML/Graphics/Rect.hpp"
#include <unordered_map>
#include <string>
#include <filesystem>

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

        void CreateScene(const std::string& ID);

        const std::unordered_map<std::string, SceneData>& GetManifest();

    private:
        std::unordered_map<std::string, SceneData> m_SceneManifest;
        std::string m_DataDir;
        std::filesystem::path m_ManifestPath;
    };
}