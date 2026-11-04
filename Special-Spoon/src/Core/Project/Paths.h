#pragma once

#include <filesystem>

namespace Spoon
{
    struct ProjectPaths
    {
        std::filesystem::path m_ProjectRoot;

        std::filesystem::path Data() const
        {
            return m_ProjectRoot / "data";
        }

        std::filesystem::path Assets() const
        {
            return m_ProjectRoot / "assets";
        }

        std::filesystem::path Presets() const
        {
            return Assets() / "presets";
        }

        std::filesystem::path Animations() const
        {
            return Assets() / "animations";
        }

        std::filesystem::path Blueprints() const
        {
            return Assets() / "blueprints";
        }
    };
}