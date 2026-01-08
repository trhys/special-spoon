#include "Serializer.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/SceneManager.h"
#include "Core/Application.h"
#include "System/SystemManager.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Spoon
{
    void SerializeScene(SceneData scene, EntityManager& e_Manager, SystemManager& s_Manager)
    {
        auto* project = Application::Get().GetProjectManager().GetCurrentProject();

        std::ofstream data(scene.DataFiles, std::ios::out | std::ios::trunc);
        if (!data.is_open())
        {
            throw std::runtime_error("Failed to open scene data file at path: " + scene.DataFiles.string());
        }

        json scenedata;
        scenedata["Entities"] = json::array();

        for (auto& [id, name] : e_Manager.GetAllEntities())
        {
            json ent;
            ent["uuid"] = id;
            ent["name"] = name;
            std::vector<Component*> comps = e_Manager.GetAllComponentsOfEntity(id);
            json entComps = json::array();
            for (auto& comp : comps)
            {
                json component = comp->Serialize();
                component["Type"] = comp->GetDisplayName();
                entComps.push_back(component);
            }
            ent["Components"] = entComps;
            scenedata["Entities"].push_back(ent);
        }

        scenedata["Systems"] = json::array();
        for (auto& system : s_Manager.GetSystems())
        {
            json sys;
            sys["Type"] = system->GetDisplayName();
            scenedata["Systems"].push_back(sys);
        }

        data << scenedata.dump(4);
        data.close();

        std::ofstream assets(scene.ResourceFiles, std::ios::out | std::ios::trunc);
        if (!assets.is_open())
        {
            throw std::runtime_error("Failed to open scene resource file at path: " + scene.ResourceFiles.string());
        }

        json sceneAssets;
        sceneAssets["Textures"] = json::array();
        sceneAssets["Fonts"] = json::array();
        sceneAssets["Sounds"] = json::array();
        sceneAssets["Animations"] = json::array();

        for (auto& [id, path] : ResourceManager::Get().GetTexturePaths())
        {
            json asset;
            asset["ID"] = id;
            if (project && !project->assetsPath.empty())
            {
                asset["FilePath"] = std::filesystem::relative(path, project->assetsPath);
            }
            else
            {
                asset["FilePath"] = path;
            }
            sceneAssets["Textures"].push_back(asset);
        }

        for (auto& [id, path] : ResourceManager::Get().GetFontPaths())
        {
            json asset;
            asset["ID"] = id;
            if (project && !project->assetsPath.empty())
            {
                asset["FilePath"] = std::filesystem::relative(path, project->assetsPath);
            }
            else
            {
                asset["FilePath"] = path;
            }
            sceneAssets["Fonts"].push_back(asset);
        }

        for (auto& [id, path] : ResourceManager::Get().GetSoundPaths())
        {
            json asset;
            asset["ID"] = id;
            if (project && !project->assetsPath.empty())
            {
                asset["FilePath"] = std::filesystem::relative(path, project->assetsPath);
            }
            else
            {
                asset["FilePath"] = path;
            }
            sceneAssets["Sounds"].push_back(asset);
        }

        for (auto& [id, data] : ResourceManager::Get().GetAnimations())
        {
            json j;
            to_json(j, data);
            sceneAssets["Animations"].push_back(j);
        }

        assets << sceneAssets.dump(4);
        assets.close();
    }

    void SerializeManifest(SceneManager& s_Manager)
    {
        std::ofstream manifest(s_Manager.GetManifestPath(), std::ios::out | std::ios::trunc);
        if (!manifest.is_open())
        {
            throw std::runtime_error("Failed to open scene manifest file at path: " + s_Manager.GetManifestPath().string());
        }

        json manifestData;
        manifestData["Scenes"] = json::array();

        for (auto& [id, scene] : s_Manager.GetManifest())
        {
            json sceneJson;
            sceneJson["ID"] = scene.ID;
            auto manifestDir = s_Manager.GetManifestPath().parent_path();
            sceneJson["ResourceFiles"] = std::filesystem::relative(scene.ResourceFiles, manifestDir);
            sceneJson["DataFiles"] = std::filesystem::relative(scene.DataFiles, manifestDir);
            manifestData["Scenes"].push_back(sceneJson);
        }

        manifest << manifestData.dump(4);
        manifest.close();
    }
}