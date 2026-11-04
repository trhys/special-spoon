#include "Editor/Editor.h"

namespace Spoon
{
    void Editor::DrawBlueprintsMenu()
    {
        if (!m_ShowBlueprintsMenu)
            return;

        // wip: the blueprint system is being refactored (see Editor/Blueprints/Blueprint.h);
        // the editor UI will return once the new EntityBlueprint format is in place.
        ImGui::SetNextWindowSize(ImVec2(420, 120), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Blueprint Creator", &m_ShowBlueprintsMenu))
        {
            ImGui::TextDisabled("Blueprints are being refactored and are temporarily unavailable.");
        }
        ImGui::End();
    }
}
