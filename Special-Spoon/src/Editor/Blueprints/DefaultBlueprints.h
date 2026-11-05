#pragma once

#include <filesystem>
#include <vector>

namespace Spoon
{
    // a built-in blueprint embedded into the binary at build time
    struct DefaultBlueprint
    {
        const char* name;   // blueprint file stem, e.g. "Player"
        const char* data;   // raw EntityBlueprint JSON
    };

    // all built-in blueprints shipped with the engine
    const std::vector<DefaultBlueprint>& GetDefaultBlueprints();

    // write any missing built-in blueprints into the given directory;
    // existing files are never overwritten so projects can customize them
    void SeedDefaultBlueprints(const std::filesystem::path& dir);
}
