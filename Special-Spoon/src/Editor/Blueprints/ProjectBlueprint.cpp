#include "ProjectBlueprint.h"
#include "Blueprint.h"

#include "Core/Serialization/JsonTypeDefs.h"
#include "ECS/Components/Core/TransformComp.h"
#include "Utils/Macros.h"

#include "project_templates.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>

namespace Spoon
{
    void from_json(const json& j, ProjectBlueprint& blueprint)
    {
        blueprint.m_SchemaVersion = j.value("schemaVersion", 1);
        blueprint.m_Name = j.value("name", "");
        blueprint.m_Profile = j.value("profile", "custom");
        blueprint.m_Scene = j.value("scene", "");
        blueprint.m_ConfigOverrides = j.value("configOverrides", json::object());
    }

    void from_json(const json& j, SceneTemplateEntity& entity)
    {
        entity.m_Blueprint = j.value("blueprint", "");
        entity.m_Name = j.value("name", "");
        if (j.contains("position"))
        {
            const auto& position = j.at("position");
            entity.m_PosX = position.value("x", 0.0f);
            entity.m_PosY = position.value("y", 0.0f);
            entity.m_HasPosition = true;
        }
    }

    void from_json(const json& j, SceneTemplate& scene)
    {
        scene.m_SchemaVersion = j.value("schemaVersion", 1);
        scene.m_Name = j.value("name", "Main");
        scene.m_Entities = j.value("entities", std::vector<SceneTemplateEntity>{});
    }

    const std::vector<BuiltInProjectTemplate>& GetBuiltInProjectTemplates()
    {
        static const std::vector<BuiltInProjectTemplate> templates = []()
        {
            std::vector<BuiltInProjectTemplate> out;
            for (const auto& entry : EmbeddedProjectTemplates::entries)
            {
                // files in subdirectories are referenced resources
                // (e.g. scene templates), not project blueprints
                if (std::string(entry.path).find('/') != std::string::npos)
                    continue;

                try
                {
                    out.push_back({ entry.path, json::parse(entry.data).get<ProjectBlueprint>() });
                }
                catch (const json::exception& e)
                {
                    SS_DEBUG_LOG("[PROJECT] Failed to parse built-in project template '"
                                 + std::string(entry.path) + "': " + e.what())
                }
            }
            return out;
        }();
        return templates;
    }

    const char* FindProjectTemplateFile(const std::string& relPath)
    {
        for (const auto& entry : EmbeddedProjectTemplates::entries)
        {
            if (relPath == entry.path)
                return entry.data;
        }
        return nullptr;
    }

    ProjectConfig ProfileDefaults(const std::string& profile)
    {
        ProjectConfig config; // engine fallback defaults
        if (profile == "isometric")
        {
            config.SortPolicy = ActiveSortPolicy::Isometric;
            config.DefaultEnabledSystems = { 
                {{"Type", "Movement"}}, 
                {{"Type", "Physics"}}, 
                {{"Type", "Collision"}}, 
                {{"Type", "Animation"}}, 
                {{"Type", "TileMapCollision"}}
            };
        }
        else if (profile == "platformer")
        {
            config.SortPolicy = ActiveSortPolicy::TopDown;
            config.DefaultEnabledSystems = { 
                {{"Type", "Movement"}}, 
                {{"Type", "Collision"}}, 
                {{"Type", "Animation"}}, 
                {{"Type", "Physics"}}
            };
        }
        else if (profile == "topdown")
        {
            config.SortPolicy = ActiveSortPolicy::TopDown;
            config.DefaultEnabledSystems = { 
                {{"Type", "Movement"}}, 
                {{"Type", "Collision"}}, 
                {{"Type", "Animation"}}, 
                {{"Type", "TileMapCollision"}},
                {{"Type", "Physics"}}
            };
        }
        // "custom" and unknown profiles keep the engine fallback defaults
        return config;
    }

    ProjectConfig ComposeProjectConfig(const std::string& profile, const json& overrides)
    {
        // precedence: explicit overrides > profile defaults > engine fallback
        json composed = ProfileDefaults(profile);
        if (overrides.is_object())
            composed.update(overrides);
        return composed.get<ProjectConfig>();
    }

    namespace
    {
        // scene templates may address blueprints as "Blueprints/Player.json";
        // strip that prefix and resolve against the project blueprints directory
        std::filesystem::path ResolveEntityBlueprint(const std::string& reference, const ProjectPaths& paths)
        {
            std::string rel = reference;
            for (const std::string prefix : { "Blueprints/", "blueprints/" })
            {
                if (rel.rfind(prefix, 0) == 0)
                {
                    rel = rel.substr(prefix.size());
                    break;
                }
            }
            return paths.Blueprints() / rel;
        }
    }

    SceneData* ScaffoldStarterScene(const ProjectBlueprint& blueprint, SceneManager& sceneManager,
                                    const ProjectPaths& paths, const ProjectConfig& config)
    {
        const char* sceneText = FindProjectTemplateFile(blueprint.m_Scene);
        if (!sceneText)
            throw std::runtime_error("scene template not found: " + blueprint.m_Scene);

        const SceneTemplate sceneTemplate = json::parse(sceneText).get<SceneTemplate>();

        // expand each entity-blueprint instance into a serialized scene entity
        json entities = json::array();
        std::uint64_t nextId = 0;
        for (const auto& instance : sceneTemplate.m_Entities)
        {
            const auto blueprintPath = ResolveEntityBlueprint(instance.m_Blueprint, paths);
            std::ifstream file(blueprintPath);
            if (!file.is_open())
            {
                SS_DEBUG_LOG("[PROJECT] Skipping starter entity -- blueprint not found: " + blueprintPath.string())
                continue;
            }

            json blueprintJson;
            file >> blueprintJson;
            const EntityBlueprint entityBlueprint = blueprintJson.get<EntityBlueprint>();

            json components = json::array();
            bool positioned = !instance.m_HasPosition;
            for (const auto& comp : entityBlueprint.m_Components)
            {
                json data = comp.m_Data;
                data["Type"] = comp.m_Name;
                if (instance.m_HasPosition && comp.m_Name == TransformComp::Name)
                {
                    data["iPos"] = { {"X", instance.m_PosX}, {"Y", instance.m_PosY} };
                    positioned = true;
                }
                components.push_back(data);
            }
            if (!positioned)
            {
                // the instance declares a position but its blueprint has no
                // transform - inject one so the placement is honored
                components.push_back({
                    {"Type", TransformComp::Name},
                    {"iPos", { {"X", instance.m_PosX}, {"Y", instance.m_PosY} }},
                    {"iScale", { {"X", 1.0f}, {"Y", 1.0f} }},
                    {"iRot", 0.0f}
                });
            }

            json entity;
            entity["uuid"] = UUID(nextId++);
            entity["name"] = instance.m_Name.empty() ? entityBlueprint.m_Name : instance.m_Name;
            entity["Components"] = components;
            entities.push_back(entity);
        }

        // create scene using the scene manager's create scene overload that takes json
        json sceneData;
        sceneData["Entities"] = entities;
        sceneData["Systems"] = config.DefaultEnabledSystems;
    
        auto* scene = sceneManager.CreateScene(sceneTemplate.m_Name.empty() ? "Main" : sceneTemplate.m_Name, sceneData);

        return scene;
    }
}
