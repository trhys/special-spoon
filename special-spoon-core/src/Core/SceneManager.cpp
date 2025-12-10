#include "SceneManager.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "ComponentLoaders.h"
#include "System/SystemManager.h"
#include "System/SystemLoaders.h"

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
                m_SceneManifest[paths.ID] = paths;
            }
        }
        else
        {
            throw std::runtime_error("Scene manifest file is missing 'Scenes' key at path: " + manifestPath);
        }
    }

    void SceneManager::LoadScene(std::string id, EntityManager& entityManager, SystemManager& systemManager)
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

        // Load animation data
        for(auto& resource : resourceData["Animations"])
        {
            AnimationData animData;
            animData.ID = resource["ID"].get<std::string>();
            animData.textureID = resource["TextureID"].get<std::string>();
            for(auto& cord : resource["SpriteCords"])
            {
                SpriteCords sc;
                sc.x = cord["x"].get<int>();
                sc.y = cord["y"].get<int>();
                sc.width = cord["width"].get<int>();
                sc.height = cord["height"].get<int>();
                animData.spriteCords.push_back(sc);
            }
            if(resource.contains("FrameRate")) { animData.frameRate = resource["FrameRate"].get<float>(); }
            if(resource.contains("Looping")) { animData.isLooping = resource["Looping"].get<bool>(); }
            ResourceManager::LoadAnimationData(animData.ID, animData);
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
            Entity newEntity = entityManager.CreateEntity();
            for(auto& comp : entity["Components"])
            {
                std::string type = comp["Type"].get<std::string>();

                SS_DEBUG_LOG("Requesting component type: " + type)

                auto& loaderMap = ComponentLoaders::GetCompLoaders();
                auto found = loaderMap.find(type);
                if(found != loaderMap.end())
                {
                    found->second(entityManager, newEntity.GetID(), comp);
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

        // Load systems
        for(auto& system : sceneData["Systems"])
        {
            systemManager.AddSystem(system);
        }
    }
}