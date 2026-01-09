#include "SceneManager.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/Application.h"
#include "ResourceManager/ResourceManager.h"
#include "Core/Registers/Component/ComponentRegistry.h"
#include "System/SystemManager.h"
#include "Utils/Macros.h"

#include "nlohmann/json.hpp"

#include <fstream>

using json = nlohmann::json;

namespace Spoon
{
    void SceneManager::LoadManifest(const std::filesystem::path& manifestPath)
    {
        //m_DataDir = manifestPath;
        m_ManifestPath = manifestPath / "scene_manifest.json";
        if(!std::filesystem::exists(m_ManifestPath.parent_path()))
        {
            std::filesystem::create_directories(m_ManifestPath.parent_path());
        }

        if(!std::filesystem::exists(m_ManifestPath))
        {
            SS_DEBUG_LOG("[SCENE MANAGER] No existing manifest found --- creating new at: " + m_ManifestPath.string())
            std::ofstream path(m_ManifestPath);

            json manifest;
            manifest["Scenes"] = json::array();
            path << manifest.dump(4);
            path.close();
        }

        std::ifstream path(m_ManifestPath);
        if(!path.is_open())
        {
            throw std::runtime_error("Failed to open scene manifest at path: " + m_ManifestPath.string());
        }
        json manifest;
        manifest = json::parse(path);
        path.close();
        if(manifest.contains("Scenes") && manifest["Scenes"].is_array())
        {
            auto manifestDir = m_ManifestPath.parent_path();
            for(auto& scene : manifest["Scenes"])
            {
                if(!scene.is_object())
                    continue;

                SceneData paths;
                paths.ID = scene.value("ID", "UnknownID");
                auto resourcePath = scene.value("ResourceFiles", "");
                auto dataPath = scene.value("DataFiles", "");
                paths.ResourceFiles = resourcePath;
                paths.DataFiles = dataPath;
                m_SceneManifest[paths.ID] = paths;
                SS_DEBUG_LOG("Registered scene in manifest: " + paths.ID)
            }
        }
        else
        {
            throw std::runtime_error("Scene manifest file is missing 'Scenes' key at path: " + m_ManifestPath.string());
        }
    }

    SceneData* SceneManager::LoadScene(std::string id, EntityManager& entityManager, SystemManager& systemManager)
    {
        UnloadScene(entityManager, systemManager);
        SS_DEBUG_LOG("Loading scene: " + id)
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
            throw std::runtime_error("Failed to open scene resource file at path: " + found->second.ResourceFiles.string());
        }

        json resourceData;
        resourceData = json::parse(resources);

        auto* project = Application::Get().GetProjectManager().GetCurrentProject();
        
        // Load textures
        for(auto& resource : resourceData["Textures"])
        {
            std::string resID = resource["ID"].get<std::string>();
            std::filesystem::path filePath = resource["FilePath"].get<std::filesystem::path>();
            
            // Resolve path relative to project assets directory
            auto fullPath = project ? project->assetsPath / filePath : filePath;
            ResourceManager::Get().LoadResource<sf::Texture>(resID, fullPath);
            SS_DEBUG_LOG("Loaded texture resource with ID: " +  resID)
        }

        try {
            // Load fonts
            for(auto& resource : resourceData["Fonts"])
            {
                std::string resID = resource["ID"].get<std::string>();
                std::filesystem::path filePath = resource["FilePath"].get<std::filesystem::path>();

                // Resolve path relative to project assets directory
                auto fullPath = project ? project->assetsPath / filePath : filePath;
                ResourceManager::Get().LoadResource<sf::Font>(resID, fullPath);
                SS_DEBUG_LOG("Loaded font resource with ID: " +  resID)
            }
        }
        catch(const json::exception& e)
        {
            throw std::runtime_error("Failed to load font resources for scene: " + id + " Error: " + e.what());
        }

        try {
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
                ResourceManager::Get().LoadAnimationData(animData.ID, animData);
                SS_DEBUG_LOG("Loaded animation data resource with ID: " +  animData.ID)
            }
        }
        catch(const json::exception& e)
        {
            throw std::runtime_error("Failed to load animation resources for scene: " + id + " Error: " + e.what());
        }

        try {
            // Load sounds
            for(auto& resource : resourceData["Sounds"])
            {
                std::string resID = resource["ID"].get<std::string>();
                std::filesystem::path filePath = resource["FilePath"].get<std::filesystem::path>();

                // Resolve path relative to project assets directory
                auto fullPath = project ? project->assetsPath / filePath : filePath;
                ResourceManager::Get().LoadResource<sf::SoundBuffer>(resID, fullPath);
                SS_DEBUG_LOG("Loaded sound resource with ID: " +  resID)
            }
        }
        catch(const json::exception& e)
        {
            throw std::runtime_error("Failed to load sound resources for scene: " + id + " Error: " + e.what());
        }

        // Begin loading scene data
        std::ifstream data(found->second.DataFiles);
        if(!data.is_open()) 
        {
            throw std::runtime_error("Failed to open scene data file at path: " + found->second.DataFiles.string());
        }

        json sceneData;
        sceneData = json::parse(data);
        try {
            // Load entities and components
            for (auto& entity : sceneData["Entities"])
            {
                UUID newID = entity["uuid"].get<UUID>();
                std::string newName = entity["name"].get<std::string>();
                SS_DEBUG_LOG("Loading entity: " + newID.ToString())
                entityManager.LoadEntity(newID, newName);

                for (auto const& data : entity["Components"])
                {
                    std::string type = data["Type"].get<std::string>();
                    SS_DEBUG_LOG("Requesting component type: " + type)
                    auto& loaderMap = ComponentRegistry::Get().GetLoaders();
                    auto found = loaderMap.find(type);
                    if (found != loaderMap.end())
                    {
                        found->second(entityManager, newID, data);
                    }
                    else
                    {
                        throw std::runtime_error("No loader found for component type: " + type);
                    }
                }
            }      
        }
        catch (const json::exception& e)
        {
            throw std::runtime_error("Failed to load entities for scene: " + id + " Error: " + e.what());
        }
        
        try {
            // Load systems
            for(auto& system : sceneData["Systems"])
            {
                std::string systemID = system["Type"].get<std::string>();
                SS_DEBUG_LOG("Loading system: " + systemID)
                systemManager.AddSystem(&system);
                SS_DEBUG_LOG("Successfully loaded system: " + systemID)
            }
        }
        catch(const json::exception& e)
        {
            throw std::runtime_error("Failed to load systems for scene: " + id + " Error: " + e.what());
        }
        SS_DEBUG_LOG("Successfully loaded scene: " + id)

        return &m_SceneManifest[id];
    }

    void SceneManager::UnloadScene(EntityManager& entityManager, SystemManager& systemManager)
    {
        SS_DEBUG_LOG("Unloading current scene...")
        ResourceManager::Get().ClearAllResources();
        entityManager.ClearArrays();
        entityManager.ClearEntities();
        systemManager.ClearSystems();
    }

    void SceneManager::Transition(std::string id, EntityManager& eManager, SystemManager& sManager)
    {
        UnloadScene(eManager, sManager);
        LoadScene(id, eManager, sManager);
    }

    const std::unordered_map<std::string, SceneData>& SceneManager::GetManifest() { return m_SceneManifest; }

    SceneData* SceneManager::CreateScene(const std::string& id)
    {
        std::ifstream inFile(m_ManifestPath);
        if(!inFile.is_open())
        {
            throw std::runtime_error("Failed to open scene manifest at path: " + m_ManifestPath.string());
        }

        json manifest = json::parse(inFile);
        inFile.close();

        std::string newID = id;
        std::replace(newID.begin(), newID.end(), ' ', '_');
        std::filesystem::path sceneDir = m_ManifestPath.parent_path() / "scenes" / newID;
        std::filesystem::create_directories(sceneDir);
        

        SceneData newScene;
        newScene.ID = newID;
        newScene.ResourceFiles = sceneDir / (newID + "_resources.json");
        newScene.DataFiles = sceneDir / (newID + "_data.json");

        json newSceneJSON;
        newSceneJSON =
        {
            {"ID", newScene.ID},
            {"ResourceFiles", newScene.ResourceFiles},
            {"DataFiles", newScene.DataFiles}
        };
        manifest["Scenes"].push_back(newSceneJSON);

        std::ofstream resources(newScene.ResourceFiles);
        json newRes;
        newRes["Textures"] = json::array();
        newRes["Fonts"] = json::array();
        newRes["Sounds"] = json::array();
        newRes["Animations"] = json::array();
        resources << newRes.dump(4);
        resources.close();

        std::ofstream data(newScene.DataFiles);
        json newData;
        newData["Entities"] = json::array();
        newData["Systems"] = json::array();
        data << newData.dump(4);
        data.close();

        m_SceneManifest[newID] = newScene;

        std::ofstream outFile(m_ManifestPath);
        if(!outFile.is_open())
        {
            throw std::runtime_error("Failed to open scene manifest at path: " + m_ManifestPath.string());
        }
        outFile << manifest.dump(4);
        outFile.close();

        SS_DEBUG_LOG("Registered new scene --- ID: " + id)

        return &m_SceneManifest[newID];
    }

    void SceneManager::DeleteScene(const std::string& id)
    {
        std::filesystem::path sceneData = m_SceneManifest[id].DataFiles;
        std::filesystem::path sceneResources = m_SceneManifest[id].ResourceFiles;
        m_SceneManifest.erase(id);
        std::ifstream inFile(m_ManifestPath);
        if(!inFile.is_open())
        {
            throw std::runtime_error("Failed to open scene manifest at path: " + m_ManifestPath.string());
        }
        json manifest = json::parse(inFile);
        inFile.close();

        auto& scenes = manifest["Scenes"];
        scenes.erase(std::remove_if(scenes.begin(), scenes.end(),
            [&id](const json& scene)
            {
                return scene["ID"].get<std::string>() == id;
            }), scenes.end());

        std::ofstream outFile(m_ManifestPath);
        if(!outFile.is_open())
        {
            throw std::runtime_error("Failed to open scene manifest at path: " + m_ManifestPath.string());
        }
        outFile << manifest.dump(4);
        outFile.close();

        SS_DEBUG_LOG("Deleted scene from manifest --- ID: " + id)

        std::ifstream dataFile(sceneData);
        if(dataFile.is_open())
        {
            dataFile.close();
            std::filesystem::remove(sceneData);
            SS_DEBUG_LOG("Deleted scene data file for scene --- ID: " + id)
        }

        std::ifstream resFile(sceneResources);
        if(resFile.is_open())
        {
            resFile.close();
            std::filesystem::remove(sceneResources);
            SS_DEBUG_LOG("Deleted scene resource file for scene --- ID: " + id)
        }

        SS_DEBUG_LOG("Deleted scene data/asset files --- ID: " + id)
    }
}