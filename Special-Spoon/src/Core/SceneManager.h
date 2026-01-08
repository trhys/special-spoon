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
        std::string ID;                         // Scene Identifier
        std::filesystem::path ResourceFiles;    // Associated Resources
        std::filesystem::path DataFiles;        // Associated Data Files
    };

    class SPOON_API SceneManager
    {
    public:
        // The scene manifest is a mapping of scene IDs to their data
        // This is loaded from disk in the project manager
        // =====================================================================================
        void LoadManifest(const std::filesystem::path& manifestPath);
        const std::unordered_map<std::string, SceneData>& GetManifest();
        const std::filesystem::path& GetManifestPath() { return m_ManifestPath; }

        // Scene data is deserialized/serialized via Serializer.h
        // Load scene deserializes scene data into EntityManager and SystemManager
        // and loads associated resources from resource files via ResourceManager
        // The scene data pointer returned is for the editor to keep track of the active scene
        // =====================================================================================
        // CreateScene creates a new scene with the given ID and adds it to the manifest
        // DeleteScene removes the scene from the manifest and deletes associated files from disk
        // =====================================================================================
        SceneData* LoadScene(std::string id, EntityManager& entityManager, SystemManager& systemManager);
        SceneData* CreateScene(const std::string& ID);
        void DeleteScene(const std::string& ID);

        // UnloadScene is called when switching scenes to clear out the EntityManager and SystemManager
        // This also clears the resource manager from loaded resources from the previous scene
        // and reloads the default assets (empty texture, default font, etc.)
        // =====================================================================================
        // Transition handles unloading the current scene and loading the new scene by ID
        // =====================================================================================
        void UnloadScene(EntityManager& entityManager, SystemManager& systemManager);
        void Transition(std::string id, EntityManager& eManager, SystemManager& sManager);

    private:
        std::unordered_map<std::string, SceneData> m_SceneManifest;
        std::filesystem::path m_DataDir;
        std::filesystem::path m_ManifestPath;
    };
}