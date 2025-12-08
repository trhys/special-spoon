#include "SceneManager.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "ComponentLoaders.h"

#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

namespace Spoon
{
    void SceneManager::LoadManifest(std::string manifestPath)
    {
        std::ifstream path(manifestPath);
        if(!path.is_open())
        {
            throw std::runtime_error("Failed to open scene manifest file at path: " + manifestPath);
        }

        json manifest;
        manifest = json::parse(path);

        if(manifest.contains("Scenes"))
        {
            for(auto& scene : manifest["Scenes"])
            {
                SceneData paths;
                paths.ID = scene["ID"].get<std::string>();
                paths.ResourceFiles = scene["Resources"].get<std::string>();
                paths.DataFiles = scene["Data"].get<std::string>();
                sf::Vector2f boundsVec(
                    scene["Bounds"]["Width"].get<float>(),
                    scene["Bounds"]["Height"].get<float>()
                );
                paths.Bounds = boundsVec;
                m_SceneManifest[paths.ID] = paths;
            }
        }
    }

    void SceneManager::LoadScene(std::string id, EntityManager& manager)
    {
        // Find scene in manifest
        auto found = m_SceneManifest.find(id);
        if(found == m_SceneManifest.end())
        {
            throw std::runtime_error("The scene you are trying to load does not exist in the manifest! : " + id);
        }

        // Begin loading resources
        std::ifstream resources(found->second.ResourceFiles);
        if(!resources.is_open())
        {
            throw std::runtime_error("Failed to open scene resource file at path: " + found->second.ResourceFiles);
        }

        json resourceData;
        resourceData = json::parse(resources);

        // Load textures
        for(auto& resource : resourceData["Textures"])
        {
            std::string resID = resource["ID"].get<std::string>();
            std::string filePath = resource["FilePath"].get<std::string>();
            ResourceManager::LoadResource<sf::Texture>(resID, filePath);
        }

        // Load fonts
        for(auto& resource : resourceData["Fonts"])
        {
            std::string resID = resource["ID"].get<std::string>();
            std::string filePath = resource["FilePath"].get<std::string>();
            ResourceManager::LoadResource<sf::Font>(resID, filePath);
        }

        // Begin loading scene data
        std::ifstream data(found->second.DataFiles);
        if(!data.is_open()) 
        {
            throw std::runtime_error("Failed to open scene data file at path: " + found->second.DataFiles);
        }

        json sceneData;
        sceneData = json::parse(data);

        try {
        // Load entities and components
        for(auto& entity : sceneData["Entities"])
        {
            Entity newEntity = manager.CreateEntity();
            for(auto& comp : entity["Components"])
            {
                std::string type = comp["Type"].get<std::string>();

                auto found = ComponentLoader::s_Loaders.find(type);
                if(found != ComponentLoader::s_Loaders.end())
                {
                    found->second(manager, newEntity.GetID(), comp);
                }
                else
                {
                    throw std::runtime_error("No loader found for component type: " + type);
                }
            }
        }

        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("Error loading scene data for scene: " + id + "\n" + e.what());
        }
    }
}