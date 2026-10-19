#include "Editor/Editor.h"

namespace Spoon
{
    void Editor::DrawMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project")) m_ShowCreateProjectPrompt = !m_ShowCreateProjectPrompt;
                if (ImGui::MenuItem("Open Project")) OpenFileDialog();
                if (ImGui::MenuItem("Save Project", nullptr, false, m_CurrentProject != nullptr)) m_ShowSaveProject = !m_ShowSaveProject;
                ImGui::Separator();
                if (ImGui::MenuItem("Project Properties", nullptr, false, m_CurrentProject != nullptr)) m_ShowConfigProject = !m_ShowConfigProject;
                if (ImGui::MenuItem("Exit")) //Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::BeginDisabled(m_CurrentProject == nullptr);
            if (ImGui::BeginMenu("Scene Manager"))
            {
                if (ImGui::MenuItem("New Scene")) NewScene = true;
                if (ImGui::MenuItem("Open Scene")) LoadScene = true;
                if (ImGui::MenuItem("Save Scene", nullptr, false, m_ActiveScene != nullptr)) SaveScene = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Scene Manifest")) ViewSceneManifest = !ViewSceneManifest;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Entity Manager"))
            {
                if (ImGui::MenuItem("Entity View")) m_ShowEntityList = !m_ShowEntityList;
                if (ImGui::MenuItem("Entity Blueprints")) m_ShowBlueprintsMenu = !m_ShowBlueprintsMenu;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Resource Manager"))
            {
                if (ImGui::MenuItem("Resource Tree")) ViewResources = !ViewResources;
                if (ImGui::MenuItem("Load Resources")) LoadResources = !LoadResources;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Animation Tool"))
            {
                if (ImGui::MenuItem("Open Animation Tool")) { m_AnimationTool.Open(nullptr); } 
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("System Manager"))
            {
                if (ImGui::MenuItem("Open System Manager", nullptr, false, m_ActiveScene != nullptr)) ViewSystemsMenu = !ViewSystemsMenu;
                ImGui::EndMenu();
            }
            ImGui::EndDisabled();

            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::SeparatorText("Editor Settings");
                ImGui::Checkbox("Display performance metrics", &m_ShowMetrics);

                ImGui::SeparatorText("Global Prompt Settings");
                ImGui::Checkbox("Confirm component delete", &m_SkipDeleteComponentPrompt);
                ImGui::Checkbox("Confirm project save", &m_SkipOverwriteSavePrompt);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Overlay"))
            {
                ImGui::Checkbox("Show Entity Selector", &m_Overlay.selectedEntityOverlay);
                ImGui::Checkbox("Show Colliders", &m_Overlay.colliderOverlay);
                ImGui::EndMenu();
            }

            // play-bar controls
            if (ImGui::Button("Play")) m_Play = true;
            if (ImGui::Button("Stop")) Stop();
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(255, 0, 0, 255));
            if (ImGui::Button("Reset")) m_ResetScene = true;
            ImGui::PopStyleColor();

            ImGui::EndMainMenuBar();
        }
    }
}