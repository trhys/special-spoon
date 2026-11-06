#include "Blueprint.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>

namespace Spoon
{
    EntityBlueprint ResolveBlueprint(const std::filesystem::path& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open blueprint file: " + path.string());
        }

        try 
        {
            json j;
            file >> j;
            return j.get<EntityBlueprint>();
        }
        catch (const json::exception& e)
        {
            throw std::runtime_error("Failed to parse blueprint file: " + path.string() + " - " + e.what());
        }
    }
}