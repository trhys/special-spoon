#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <filesystem>
#include <vector>
#include <unordered_map>
#include <string>

namespace Spoon
{
    struct UserConfig
    {
        bool EnableAutoSave = true;                         // Enable or disable auto-saving of projects
        int AutoSaveIntervalMinutes = 10;                   // Interval in minutes for auto-saving
        std::vector<std::filesystem::path> RecentProjects;  // List of recently opened projects
		std::unordered_map<std::filesystem::path, std::vector<std::string>> RecentScenes; // Map of recent files for each project
    };

    UserConfig& LoadUserConfig();
    void SaveUserConfig(const UserConfig& config);

    inline void to_json(json& j, const UserConfig& config)
    {
        j = json{
            {"EnableAutoSave", config.EnableAutoSave},
            {"AutoSaveIntervalMinutes", config.AutoSaveIntervalMinutes},
            {"RecentProjects", config.RecentProjects},
            {"RecentScenes", config.RecentScenes}
        };
	}

    inline void from_json(const json& j, UserConfig& config)
    {
        j.at("EnableAutoSave").get_to(config.EnableAutoSave);
        j.at("AutoSaveIntervalMinutes").get_to(config.AutoSaveIntervalMinutes);
        j.at("RecentProjects").get_to(config.RecentProjects);
        j.at("RecentScenes").get_to(config.RecentScenes);
	}
}