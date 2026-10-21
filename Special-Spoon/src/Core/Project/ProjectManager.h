#pragma once

#include "Core/Core.h"
#include "Core/Project/Config.h"
#include "Editor/Blueprints/Blueprint.h"

#include <filesystem>
#include <string>
#include <memory>
#include <vector>

namespace Spoon
{
    struct Project
    {
        std::string name;                                   // Unique identifier for the project
        std::filesystem::path filePath;                     // Path to the project file
        std::filesystem::path dataPath;                     // Path to the project data directory
        std::filesystem::path assetsPath;                   // Path to the project assets directory
        std::filesystem::path presetsPath;                  // Path to the project presets directory
        std::string version = "1.0";                        // Project version
        std::vector<std::filesystem::path> recentFiles;     // List of recently opened files
        ProjectConfig config;                               // Config settings for project
        std::vector<Blueprint> blueprints;                  // Project prefabs
    };
    
    class SPOON_API ProjectManager
    {
    public:    
        Project* CreateNew(std::string newProjectBuf);
        Project* OpenFromFile(const std::filesystem::path& filepath);
		Project* SaveProject(Project* project);
        void DeleteProject(const std::filesystem::path& filepath);
        Project* GetCurrentProject() { return m_CurrentProject.get(); }
        std::vector<Blueprint>& GetBlueprints();
        std::vector<std::string> GetAvailablePresets(const std::string& category);

    private:
        std::unique_ptr<Project> m_CurrentProject = nullptr;
    };
}
