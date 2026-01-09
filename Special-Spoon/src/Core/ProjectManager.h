#pragma once

#include "Core.h"
#include <filesystem>
#include <string>
#include <memory>
#include <vector>

namespace Spoon
{
    class Editor;

    struct Project
    {
        std::string ID;                                     // Unique identifier for the project
        std::filesystem::path filePath;                     // Path to the project file
        std::filesystem::path dataPath;                     // Path to the project data directory
        std::filesystem::path assetsPath;                   // Path to the project assets directory
        std::string version = "1.0";                        // Project version
        std::vector<std::filesystem::path> recentFiles;     // List of recently opened files
    };
    
    class SPOON_API ProjectManager
    {
    public:
        ProjectManager() {}
        ~ProjectManager() {}

        void CreateNew(Editor* editor);
        void LoadProject(Editor* editor);
        void SaveProject(Editor* editor);
        void OpenFromFile(const std::filesystem::path& filepath);

        Project* GetCurrentProject() { return m_CurrentProject.get(); }

    private:
        std::unique_ptr<Project> m_CurrentProject = nullptr;
        bool m_Saving = false;
    };
}