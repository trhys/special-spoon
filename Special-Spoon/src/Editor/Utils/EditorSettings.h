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

        bool skipAskBeforeDeleteComp = false;        // Skip prompt before deleting component in entity inspector
        bool skipProjectSavePrompt = false;         // Skip save project prompt
        bool displayEditorMetrics = true;           // Display editor metrics window
        bool displayImGuiMetrics = false;           // Display ImGui metrics window
        bool showColliderOverlay = false;           // Draw collider bounds as gizmo overlays in the viewport

    private:
        EditorSettings() = default;
    };
}