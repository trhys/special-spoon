#include "Editor.h"
#include "Core/EntityManager.h"
#include "Core/SceneManager.h"
#include "System/SystemManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    static void HelpMarker(const char* desc);

    bool LoadScene = false;
    bool ViewEntities = false;

    void Editor::Close() 
    {

    }

    void Editor::Run(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
        ImGui::Begin("Special-Spoon Editor");

        if(ImGui::Button("Load Scene"))
        {
            LoadScene = true;
        }

        if(LoadScene)
        {
            const auto& scenes = s_Manager.GetManifest();
            static std::string selectedScene = "";

            ImGui::BeginListBox("Scene Manifest");
            for(const auto& [name, sceneData] : scenes)
            {
                const bool is_selected = (selectedScene == name);
                if(ImGui::Selectable(name.c_str(), is_selected))
                    selectedScene = name;

                if(is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of available scenes in the manifest");

            if(ImGui::Button("Select"))
            {
                s_Manager.LoadScene(selectedScene, e_Manager, sys_Manager);
                LoadScene = false;
            }
        }

        if(ImGui::Button("View entities"))
        {
            ViewEntities = true;
        }

        if(ViewEntities)
        {
            // Display a selectable list of active entities
            const auto& entities = e_Manager.GetAllEntities();
            static UUID selectedID = 0;

            ImGui::BeginListBox("Entities");
            for(const auto& [uuid, name] : entities)
            {
                const bool is_selected = (selectedID == uuid);
                if (ImGui::Selectable(name.c_str(), is_selected))
                    selectedID = uuid;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of all active entities in the scene");
        }
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