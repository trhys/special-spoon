#include "ProjectManager.h"
#include "Core/Application.h"
#include "Core/Serialization/Serializer.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Spoon
{
    Project* ProjectManager::CreateNew(std::string newProjectBuf)
    {
        const std::filesystem::path projectRoot = std::filesystem::path("projects") / newProjectBuf;
        if (std::filesystem::exists(projectRoot))
        {
            SS_DEBUG_LOG("[PROJECT] Project already exists: " + projectRoot.string())
            return nullptr;
        }

        SS_DEBUG_LOG("[PROJECT] Creating new project: " + projectRoot.string())
        // create new project structure
        Project newProject;
        newProject.name = newProjectBuf;
        newProject.paths.m_ProjectRoot = projectRoot;

        // Create project dir - subdirectories are lazy loaded
        std::filesystem::create_directories(newProject.paths.m_ProjectRoot);
        newProject.version = "1.0";

        // Open the project file for writing
        std::ofstream newProj(newProject.paths.m_ProjectRoot / (newProject.name + ".sproj"));
        if (!newProj.is_open())
        {
            SS_DEBUG_LOG("[PROJECT] Failed to create new project file: " + std::string((newProject.paths.m_ProjectRoot / (newProject.name + ".sproj")).string()))
            return nullptr;
        }

        try
        {
            // Dump data to JSON
            json projectJson;
            projectJson["name"] = newProject.name;
            projectJson["Version"] = newProject.version;
            projectJson["Config"] = newProject.config;
            newProj << projectJson.dump(4);
            newProj.close();
        }
        catch (const json::exception& e)
        {
            SS_DEBUG_LOG("[PROJECT] Failed to write project JSON: " + std::string(e.what()));
            newProj.close();
            std::filesystem::remove(newProject.paths.m_ProjectRoot / (newProject.name + ".sproj"));
            return nullptr;
        }

        m_CurrentProject = std::make_unique<Project>(newProject);
        Application::Get().OnProjectLoad();
        return m_CurrentProject.get();
    }

    Project* ProjectManager::OpenFromFile(const std::filesystem::path& filepath)
    {
        if (!std::filesystem::exists(filepath))
        {
            SS_DEBUG_LOG("[PROJECT] Project file cannot be found: " + filepath.string())
            return nullptr;
        }
        if (filepath.extension() != ".sproj")
        {
            SS_DEBUG_LOG("[PROJECT] Must select a .sproj file - got: " + filepath.string())
            return nullptr;
        }
        std::ifstream fileStream(filepath);
        if (!fileStream.is_open())
        {
            SS_DEBUG_LOG("[PROJECT] Failed to open project file: " + filepath.string())
            return nullptr;
        }

        try {
            // Deserialize project data from the file
            json projectJson;
            projectJson = json::parse(fileStream);
            fileStream.close();

            Project newProject;
            newProject.name = projectJson.value("name", "UnknownProject");
            newProject.version = projectJson.value("Version", "1.0");
			newProject.config = projectJson.value("Config", ProjectConfig{});
            newProject.paths.m_ProjectRoot = filepath.parent_path();

            Application::Get().GetSceneManager().LoadManifest(newProject.paths.Data().string());
            try {
                ResourceManager::Get().ScanAssets(newProject.paths.Assets());
            } catch (const std::filesystem::filesystem_error& e)
            {
                SS_DEBUG_LOG("[PROJECT] Failed to scan assets on project load: " + std::string(e.what()))
                ResourceManager::Get().ResetAssetsDirectory(newProject.paths.Assets());
            }
            m_CurrentProject = std::make_unique<Project>(newProject);
        }
        catch (const json::exception& e)
        {
            SS_DEBUG_LOG("[PROJECT] Failed to parse project JSON: " + std::string(e.what()))
            fileStream.close();
            return nullptr;
        }
        Application::Get().OnProjectLoad();
        return m_CurrentProject.get();
    }

    Project* ProjectManager::SaveProject(Project* project)
    {
        if (!project)
            return nullptr;

        try {
            json projectJson;
            projectJson["name"] = project->name;
            projectJson["Version"] = project->version;
            projectJson["Config"] = project->config;

            const std::string serializedProject = projectJson.dump(4);
            std::ofstream fileStream(project->paths.m_ProjectRoot / (project->name + ".sproj"), std::ios::out | std::ios::trunc);
            if (!fileStream.is_open())          
                return nullptr;

            fileStream << serializedProject;
            fileStream.close();
            return project;
        }
        catch (const std::exception& e)
        {
            SS_DEBUG_LOG("[PROJECT] Failed to save project: " + std::string(e.what()))
            return nullptr;
        }
    }
}
