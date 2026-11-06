#pragma once

#include "Core/Project/Config.h"
#include "Core/Project/Paths.h"
#include "Core/SceneManager.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <string>
#include <vector>

namespace Spoon
{
    // a reusable starter-project template, distinct from entity blueprints:
    // declares a project profile, a starter scene template and optional
    // project-config overrides
    struct ProjectBlueprint
    {
        int m_SchemaVersion = 1;
        std::string m_Profile;          // isometric | platformer | topdown | custom
        std::string m_Name;             // display name, e.g. "Platformer Starter"
        std::string m_Scene;            // starter scene template, path relative to the template set
        json m_ConfigOverrides = json::object();
    };

    // a single entity-blueprint instance inside a starter scene template
    struct SceneTemplateEntity
    {
        std::string m_Blueprint;        // entity blueprint path, resolved against the project blueprints dir
        std::string m_Name;
        float m_PosX = 0.0f;
        float m_PosY = 0.0f;
        bool m_HasPosition = false;
    };

    // the starter scene described by a project blueprint
    struct SceneTemplate
    {
        int m_SchemaVersion = 1;
        std::string m_Name;
        std::vector<SceneTemplateEntity> m_Entities;
    };

    void from_json(const json& j, ProjectBlueprint& blueprint);
    void from_json(const json& j, SceneTemplateEntity& entity);
    void from_json(const json& j, SceneTemplate& scene);

    // a built-in project blueprint embedded at build time
    struct BuiltInProjectTemplate
    {
        std::string m_File;             // embedded file key, e.g. "Isometric-Starter.json"
        ProjectBlueprint m_Blueprint;
    };

    // all built-in project blueprints shipped with the engine
    const std::vector<BuiltInProjectTemplate>& GetBuiltInProjectTemplates();

    // raw JSON of an embedded project-template file (e.g. "Scenes/..."), or nullptr
    const char* FindProjectTemplateFile(const std::string& relPath);

    // profile-derived ProjectConfig defaults; unknown/custom profiles keep
    // the engine fallback defaults. kept in the composition layer so core
    // systems never branch on the profile
    ProjectConfig ProfileDefaults(const std::string& profile);

    // compose a project config with the documented precedence:
    // explicit overrides > profile defaults > engine fallback
    ProjectConfig ComposeProjectConfig(const std::string& profile, const json& overrides);

    // scaffold the blueprint's starter scene into the current project:
    // creates the scene files and expands the entity-blueprint instances
    // into serialized scene entities; throws on unrecoverable failures
    SceneData* ScaffoldStarterScene(
        const ProjectBlueprint& blueprint, 
        SceneManager& sceneManager,
        const ProjectPaths& paths,
        const ProjectConfig& config
    );
}
