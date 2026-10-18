#include "ProjectManager.h"
#include "Core/Application.h"
#include "Core/Serialization/Serializer.h"
#include "Editor/Blueprints/Blueprint.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Spoon
{
    Project* ProjectManager::CreateNew(std::string newProjectBuf)
    {
        // create new project structure
        Project newProject;
        newProject.name = newProjectBuf;
        newProject.blueprints = GetDefaultBlueprints();

        // Create dirs
        std::filesystem::create_directories("projects/");
        newProject.filePath = std::filesystem::path("projects/") / (newProject.name + ".json");

        // Open the project file for writing
        std::ofstream newProj(newProject.filePath);
        if (!newProj.is_open())
        {
            return nullptr;
        }

        newProject.dataPath = std::filesystem::path("projects/") / newProject.name / "data/";
        std::filesystem::create_directories(newProject.dataPath);
        newProject.assetsPath = std::filesystem::path("projects/") / newProject.name / "assets/";
        std::filesystem::create_directories(newProject.assetsPath);
        newProject.presetsPath = newProject.assetsPath / "presets/";
        std::filesystem::create_directories(newProject.presetsPath);
        newProject.version = "1.0";

        // Dump data to JSON
        json projectJson;
        projectJson["name"] = newProject.name;
        projectJson["DataPath"] = newProject.dataPath;
        projectJson["AssetsPath"] = newProject.assetsPath;
        projectJson["PresetsPath"] = newProject.presetsPath;
        projectJson["Version"] = newProject.version;
        projectJson["RecentFiles"] = newProject.recentFiles;
        projectJson["Config"] = ProjectConfig{};
        projectJson["Blueprints"] = newProject.blueprints; 
        newProj << projectJson.dump(4);
        newProj.close();

        m_CurrentProject = std::make_unique<Project>(newProject);
        return m_CurrentProject.get();
    }

    Project* ProjectManager::OpenFromFile(const std::filesystem::path& filepath)
    {
        std::ifstream fileStream(filepath);
        if (!fileStream.is_open())
        {

            return nullptr;
        }

        try {
            // Deserialize project data from the file
            json projectJson;
            projectJson = json::parse(fileStream);
            fileStream.close();

            Project newProject;
            newProject.name = projectJson.value("name", "UnknownProject");
            newProject.filePath = filepath;
            newProject.dataPath = projectJson.value("DataPath", "");
            newProject.assetsPath = projectJson.value("AssetsPath", "");
            newProject.presetsPath = projectJson.value("PresetsPath", newProject.assetsPath / "presets/");
            newProject.version = projectJson.value("Version", "1.0");
            newProject.recentFiles = projectJson.value("RecentFiles", std::vector<std::filesystem::path>{});
			newProject.config = projectJson.value("Config", ProjectConfig{});
            newProject.blueprints = projectJson.value("Blueprints", GetDefaultBlueprints());

            Application::Get().GetSceneManager().LoadManifest(newProject.dataPath.string());
            ResourceManager::Get().ScanAssets(newProject.assetsPath);
            m_CurrentProject = std::make_unique<Project>(newProject);
        }
        catch (const json::exception& e)
        {
            // Handle JSON parsing error
            fileStream.close();
            return nullptr;
        }
        return m_CurrentProject.get();
    }

    Project* ProjectManager::SaveProject(Project* project)
    {
        if (!project)
            return nullptr;

        json projectJson;
        projectJson["name"] = project->name;
        projectJson["DataPath"] = project->dataPath.string();
        projectJson["AssetsPath"] = project->assetsPath.string();
        projectJson["PresetsPath"] = project->presetsPath.string();
        projectJson["Version"] = project->version;
		projectJson["Config"] = project->config;
        projectJson["Blueprints"] = project->blueprints;

        const std::string serializedProject = projectJson.dump(4);
        std::ofstream fileStream(project->filePath, std::ios::out | std::ios::trunc);
        if (!fileStream.is_open())          
            return nullptr;

        fileStream << serializedProject;
        fileStream.close();
        return project;
    }

    std::vector<Blueprint>& ProjectManager::GetBlueprints()
    {
        if (!m_CurrentProject)
            throw std::runtime_error("fetching blueprints with no project loaded");

        return m_CurrentProject->blueprints;
    }

    // get all presets from the assets folder by category
    // ex: GetAvailablePresets("particles") will return all JSON presets in the "particles" category folder.
    std::vector<std::string> ProjectManager::GetAvailablePresets(const std::string& category)
    {
        if (!m_CurrentProject)
            throw std::runtime_error("fetching available presets with no project loaded");

        // verify category is a valname dir
        if (!std::filesystem::exists(m_CurrentProject->presetsPath / category) || !std::filesystem::is_directory(m_CurrentProject->presetsPath / category))
            throw std::runtime_error("invalname presets category: " + category);

        std::vector<std::string> presets;
        for (const auto& entry : std::filesystem::directory_iterator(m_CurrentProject->presetsPath / category))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                presets.push_back(entry.path().stem().string());
            }
        }
        return presets;
    }
}
