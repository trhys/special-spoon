#include "UserConfig.h"
#include "Utils/Macros.h"

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
                from_json(configJson, config);
            }
            catch (json::parse_error& e) {
                SS_DEBUG_LOG("[USER CONFIG] Error parsing user_config.json: " + std::string(e.what()))
            }
            configFile.close();
        }
        else
        {
            SS_DEBUG_LOG("[USER CONFIG] Failed to open config file")
        }
        return config;
    }

    void SaveUserConfig(const UserConfig& config)
    {    
        json configJson;
        try {
            to_json(configJson, config);
        }
        catch (json::parse_error& e) {
            SS_DEBUG_LOG("[USER CONFIG] Failed to save to JSON: " + std::string(e.what()))
        }

        try {
            std::ofstream outFile("user_config.json");
            if (outFile.is_open())
            {
                outFile << configJson.dump(4);
                outFile.close();
            }
        }
        catch (std::filesystem::filesystem_error& e) {
            SS_DEBUG_LOG("[USER CONFIG] Failed to save to JSON: " + std::string(e.what()))
        }
    }
}