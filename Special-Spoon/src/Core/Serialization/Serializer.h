#pragma once

#include "Core/EntityManager.h"
#include <Core/SceneManager.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Spoon
{
    static inline void Serialize(SceneData scene, EntityManager& e_Manager, SystemManager& s_Manager)
    {
        std::ofstream data(scene.DataFiles, std::ios::out | std::ios::trunc);
        if (!data.is_open())
        {
            throw std::runtime_error("Failed to open scene data file at path: " + scene.DataFiles);
        }

        json scenedata;
        scenedata["Entities"] = json::array();

        for (auto& [id, name] : e_Manager.GetEntities())
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

        data << scenedata.dump(4);
        data.close();

        std::ofstream assets(scene.ResourceFiles, std::ios::out | std::ios::trunc);
        if (!assets.is_open())
        {
            throw std::runtime_error("Failed to open scene resource file at path: " + scene.ResourceFiles);
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
            asset["FilePath"] = path;
            sceneAssets["Textures"].push_back(asset);
        }

        for (auto& [id, path] : ResourceManager::Get().GetFontPaths())
        {
            json asset;
            asset["ID"] = id;
            asset["FilePath"] = path;
            sceneAssets["Fonts"].push_back(asset);
        }

        for (auto& [id, path] : ResourceManager::Get().GetSoundPaths())
        {
            json asset;
            asset["ID"] = id;
            asset["FilePath"] = path;
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
}