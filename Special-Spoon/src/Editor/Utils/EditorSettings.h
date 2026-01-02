#pragma once

namespace Spoon
{
    struct EditorSettings
    {
        static EditorSettings& Get()
        {
            static EditorSettings settings;
            return settings;
        }
        EditorSettings(const EditorSettings&) = delete;
        EditorSettings& operator=(const EditorSettings&) = delete;
        EditorSettings(EditorSettings&&) = delete;
        EditorSettings& operator=(EditorSettings&&) = delete;

        bool compDelAskAgain = true;        // Ask before deleting component in entity inspector

    private:
        EditorSettings() = default;
    };
}