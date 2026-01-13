#include "UserConfig.h"
#include "Macros.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace Spoon
{
    UserConfig& LoadUserConfig()
    {
        static UserConfig config;
        std::ifstream configFile("user_config.json");
        if (configFile.is_open())
        {
            try {
                json configJson;
                configJson = json::parse(configFile);
                config.EnableAutoSave = configJson.value("EnableAutoSave", true);
                config.AutoSaveIntervalMinutes = configJson.value("AutoSaveIntervalMinutes", 10);
                config.RecentProjects = configJson.value("RecentProjects", std::vector<std::filesystem::path>{});
            }
            catch (json::parse_error& e) {
                //SS_DEBUG_LOG("Error parsing user_config.json: {}" + e.what())
            }
            configFile.close();
        }
        else
        {
            // If config file doesn't exist, create one with default settings
            json configJson;
            configJson["EnableAutoSave"] = config.EnableAutoSave;
            configJson["AutoSaveIntervalMinutes"] = config.AutoSaveIntervalMinutes;
            configJson["RecentProjects"] = config.RecentProjects;

            std::ofstream outFile("user_config.json");
            if (outFile.is_open())
            {
                outFile << configJson.dump(4);
                outFile.close();
            }
        }
        return config;
    }

    void SaveUserConfig(const UserConfig& config)
    {
        json configJson;
        configJson["EnableAutoSave"] = config.EnableAutoSave;
        configJson["AutoSaveIntervalMinutes"] = config.AutoSaveIntervalMinutes;
        configJson["RecentProjects"] = config.RecentProjects;

        std::ofstream outFile("user_config.json");
        if (outFile.is_open())
        {
            outFile << configJson.dump(4);
            outFile.close();
        }
    }
}