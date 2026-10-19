#include "Editor/Editor.h"
#include "Editor/Utils/Windows.h"
#include "Core/Serialization/Serializer.h"

namespace Spoon
{
    void Editor::DrawNewScene()
    {
        if (!m_ShowNewScene)
            return;

        if (GetActiveScene()) {
            m_ShowSaveScene = true;
            return;
        }

        static char newSceneBuf[64] = {0};
        auto clear = [&]()
        {
            newSceneBuf[0] = '\0';
            m_ShowNewScene = false;
        };

        CenterNext();        
        if(ImGui::Begin("New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter scene ID: "); ImGui::SameLine();

            if (ImGui::IsWindowAppearing())
                ImGui::SetKeyboardFocusHere();

            if(ImGui::InputText("##ID", newSceneBuf, IM_ARRAYSIZE(newSceneBuf), ImGuiInputTextFlags_EnterReturnsTrue) && strlen(newSceneBuf) > 0 )
            {
                SetActiveScene(m_Services.sceneManager->CreateScene(newSceneBuf));
                clear();
            }
            if(ImGui::Button("Submit") && strlen(newSceneBuf) > 0 )
            {
                SetActiveScene(m_Services.sceneManager->CreateScene(newSceneBuf));
                clear();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                clear();
            }
        }
        ImGui::End();
    }

    void Editor::DrawSaveScene()
    {
        if (!m_ShowSaveScene)
            return;

        const char* popupName = "Prompt Save Before New Scene";
        if (!ImGui::IsPopupOpen(popupName))
            ImGui::OpenPopup(popupName);

        CenterNext();
        if (ImGui::BeginPopupModal(popupName, NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Do you want to save the active scene?\n"
                        "This will overwrite any existing scene file.");
            ImGui::Separator();
            if (ImGui::Button("Save and Continue"))
            {
                if (GetActiveScene())
                {
                    SerializeScene(*GetActiveScene(), *m_Services.entityManager, *m_Services.systemManager);
                    if (m_ShowNewScene)
                    {
                        m_Services.sceneManager->UnloadScene(*m_Services.entityManager, *m_Services.systemManager);
                        SetActiveScene(nullptr);
                    }
                }
                m_ShowSaveScene = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                m_ShowSaveScene = false;
                m_ShowNewScene = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Exit Scene w/o Saving"))
            {
                m_ShowSaveScene = false;
                m_Services.sceneManager->UnloadScene(*m_Services.entityManager, *m_Services.systemManager);
                SetActiveScene(nullptr);
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("All unsaved changes will be lost!");
            ImGui::EndPopup();
        }
    }

    void Editor::DrawLoadScene()
    {
        if (!m_ShowLoadScene)
            return;

        CenterNext();
        ImGui::Begin("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        const auto& scenes = m_Services.sceneManager->GetManifest();
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
            SetActiveScene(m_Services.sceneManager->LoadScene(selectedScene, *m_Services.entityManager, *m_Services.systemManager));
            m_ShowLoadScene = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")) 
            m_ShowLoadScene = false;
        
        ImGui::End();
    }

    void Editor::DrawDeleteScenes()
    {
        if (!m_ShowDeleteScene)
            return;
        static std::unordered_map<std::string, bool> scenesToDelete;

        CenterNext();
        ImGui::Begin("Scene Manifest", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        const auto& scenes = m_Services.sceneManager->GetManifest();

        if (ImGui::BeginListBox("Scene Manifest"))
        {
            int index = 0;
            for (const auto& [name, sceneData] : scenes)
            {
                ImGui::Text("Scene #%d : %s", index, sceneData.ID.c_str());
                ImGui::SameLine();
                ImGui::Checkbox(("##delete" + sceneData.ID).c_str(), &scenesToDelete[sceneData.ID]);
                index++;
            }
            ImGui::EndListBox();
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("Delete Scenes"))
        {
            auto activeScene = GetActiveScene();
            for (const auto& [sceneID, toDelete] : scenesToDelete)
            {
                if (toDelete)
                {
                    if (activeScene && activeScene->ID == sceneID)
                        SetActiveScene(nullptr);
                    m_Services.sceneManager->DeleteScene(sceneID);
                }
            }
        }
        ImGui::SameLine(); HelpMarker("Permanent deletion of selected scenes. Cannot be undone");
        ImGui::PopStyleColor();
            
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            scenesToDelete.clear();
            m_ShowDeleteScene = false;
        }

        ImGui::End();
    }
}