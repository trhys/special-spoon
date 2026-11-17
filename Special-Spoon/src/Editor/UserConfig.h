#pragma once

#include <filesystem>
#include <vector>

namespace Spoon
{
    struct UserConfig
    {
        bool EnableAutoSave = true;                         // Enable or disable auto-saving of projects
        int AutoSaveIntervalMinutes = 10;                   // Interval in minutes for auto-saving
        std::vector<std::filesystem::path> RecentProjects;  // List of recently opened projects
    };

    UserConfig& LoadUserConfig();
    void SaveUserConfig(const UserConfig& config);
}