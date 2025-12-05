#include "SceneManager.h"
#include "ResourceManager.h"

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
                FilePaths paths;
                paths.ID = scene["ID"].get<std::string>();
                paths.ResourceFiles = scene["Resources"].get<std::string>();
                paths.DataFiles = scene["Data"].get<std::string>();
                m_SceneManifest[paths.ID] = paths;
            }
        }
    }

    void SceneManager::LoadScene(std::string id)
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

        for(auto& resource : resourceData["Textures"])
        {
            std::string resID = resource["ID"].get<std::string>();
            std::string filePath = resource["FilePath"].get<std::string>();
            ResourceManager::LoadResource<sf::Texture>(resID, filePath);
        }

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
        
        for(auto& entity : sceneData["Entities"])
        {
            // TODO Load entity data here
        }
    }
}