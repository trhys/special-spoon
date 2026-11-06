#include "DefaultBlueprints.h"

#include "Utils/Macros.h"

#include "default_blueprints.h"

#include <fstream>
#include <string>

namespace Spoon
{
    const std::vector<DefaultBlueprint>& GetDefaultBlueprints()
    {
        static const std::vector<DefaultBlueprint> blueprints = []()
        {
            std::vector<DefaultBlueprint> out;
            for (const auto& entry : EmbeddedBlueprints::entries)
                out.push_back({ entry.name, entry.data });
            return out;
        }();
        return blueprints;
    }

    void SeedDefaultBlueprints(const std::filesystem::path& dir)
    {
        std::filesystem::create_directories(dir);
        for (const auto& blueprint : GetDefaultBlueprints())
        {
            const auto path = dir / (std::string(blueprint.name) + ".json");
            if (std::filesystem::exists(path))
                continue; // never overwrite project customizations

            std::ofstream file(path, std::ios::out | std::ios::trunc);
            if (!file.is_open())
            {
                SS_DEBUG_LOG("[BLUEPRINTS] Failed to seed default blueprint: " + path.string())
                continue;
            }
            file << blueprint.data;
            SS_DEBUG_LOG("[BLUEPRINTS] Seeded default blueprint: " + path.string())
        }
    }
}
