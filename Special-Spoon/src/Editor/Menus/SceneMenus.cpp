#include "SceneMenus.h"
#include "Editor/Utils/EditorSettings.h"
#include "Editor/Utils/Helpmarker.h"

#include "Core/SceneManager.h"
#include "Core/EntityManager.h"

#include "System/SystemManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    void NewSceneMenu(SceneManager& s_Manager, Editor* editor)
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        const char* popupId = "New Scene";
        static char newSceneBuf[64] = {0};
        if (!ImGui::IsPopupOpen(popupId))
            ImGui::OpenPopup(popupId);

        if(ImGui::BeginPopupModal(popupId, &editor->NewScene, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter scene ID: "); ImGui::SameLine();

            if (ImGui::IsWindowAppearing())
                ImGui::SetKeyboardFocusHere();

            if(ImGui::InputText("##ID", newSceneBuf, IM_ARRAYSIZE(newSceneBuf), ImGuiInputTextFlags_EnterReturnsTrue) && strlen(newSceneBuf) > 0 )
            {
                editor->SetActiveScene(s_Manager.CreateScene(newSceneBuf));
                newSceneBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
                editor->NewScene = false;
            }
            if(ImGui::Button("Submit") && strlen(newSceneBuf) > 0 )
            {
                editor->SetActiveScene(s_Manager.CreateScene(newSceneBuf));
                newSceneBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
                editor->NewScene = false;
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                newSceneBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
                editor->NewScene = false;
            }
            ImGui::EndPopup();
        }
    }

    void LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager, Editor* editor)
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        ImGui::Begin("Load Scene", &editor->LoadScene);
        const auto& scenes = s_Manager.GetManifest();
        static std::string selectedScene = "";

        if (ImGui::BeginListBox("Scene Manifest"))
        {
            for (const auto& [name, sceneData] : scenes)
            {
                const bool is_selected = (selectedScene == name);
                if (ImGui::Selectable(name.c_str(), is_selected))
                    selectedScene = name;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of available scenes in the manifest");
        }
        
        if(ImGui::Button("Open"))
        {
            editor->SetActiveScene(s_Manager.LoadScene(selectedScene, e_Manager, sys_Manager));
            editor->LoadScene = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")) 
            editor->LoadScene = false;
        
        ImGui::End();
    }
}