#pragma once

#include "Paths.h"
#include "Core/Core.h"
#include "Core/Project/Config.h"

#include <filesystem>
#include <string>
#include <memory>
#include <vector>

namespace Spoon
{
    struct Project
    {
        std::string name;              // Unique identifier for the project
        std::string version = "1.0";   // Project version
        ProjectPaths paths;            // Project directory paths
        ProjectConfig config;          // Config settings for project
    };
    
    class SPOON_API ProjectManager
    {
    public:    
        Project* CreateNew(std::string newProjectBuf);
        Project* OpenFromFile(const std::filesystem::path& filepath);
		Project* SaveProject(Project* project);
        Project* GetCurrentProject() { return m_CurrentProject.get(); }
        ProjectPaths GetCurrentProjectPaths() { return m_CurrentProject ? m_CurrentProject->paths : ProjectPaths{}; }
        
    private:
        std::unique_ptr<Project> m_CurrentProject = nullptr;
    };
}
