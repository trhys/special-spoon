#include "Editor.h"
#include "Core/EntityManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    static void HelpMarker(const char* desc);

    void Editor::Close() 
    {

    }

    void Editor::Run(EntityManager& manager)
    {
        ImGui::Begin("Special-Spoon Editor");

        // Display a selectable list of active entities
        const auto entities = manager.GetAllEntities();
        static UUID selectedID = 0;

        ImGui::BeginListBox("Entities");
        for(auto [uuid, name] : entities)
        {
            const bool is_selected = (selectedID == uuid);
            if (ImGui::Selectable(name.c_str(), is_selected))
                selectedID = uuid;

            if (ImGui::IsItemHovered())
                selectedID = uuid;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
        ImGui::SameLine(); HelpMarker("A list of all active entities in the scene");
    }

    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}