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
                if (ImGui::MenuItem("Open Project")) OpenFileDialog(DialogOption::Open);
                if (ImGui::MenuItem("Save Project", nullptr, false, m_CurrentProject != nullptr)) m_ShowOverwriteProjectPrompt = true;
                if (ImGui::MenuItem("Delete Project")) OpenFileDialog(DialogOption::Delete);
                ImGui::Separator();
                if (ImGui::MenuItem("Project Properties", nullptr, false, m_CurrentProject != nullptr)) m_ShowConfigProject = !m_ShowConfigProject;
                if (ImGui::MenuItem("Exit")) RequestQuit();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene Manager"))
            {
                if (ImGui::MenuItem("New Scene")) m_ShowNewScene = !m_ShowNewScene;
                if (ImGui::MenuItem("Open Scene")) m_ShowLoadScene = !m_ShowLoadScene;
                if (ImGui::MenuItem("Save Scene", nullptr, false, m_ActiveScene != nullptr)) m_ShowSaveScene = !m_ShowSaveScene;
                if (ImGui::MenuItem("Delete Scenes")) m_ShowDeleteScene = !m_ShowDeleteScene;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Entity List")) m_ShowEntityList = !m_ShowEntityList;
                if (ImGui::MenuItem("Blueprints")) m_ShowBlueprintsMenu = !m_ShowBlueprintsMenu;
                ImGui::SeparatorText("Resources");
                if (ImGui::MenuItem("Resource Manager")) m_ShowResourcesMenu = !m_ShowResourcesMenu;
                ImGui::SeparatorText("Systems");
                if (ImGui::MenuItem("System Manager", nullptr, false, m_ActiveScene != nullptr)) m_ShowSystemsMenu = !m_ShowSystemsMenu;
                ImGui::SeparatorText("Utilities");
                if (ImGui::MenuItem("Log Console")) m_ShowLogger = !m_ShowLogger;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Animation Tool")) { m_AnimationTool.Open(nullptr); } 
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::SeparatorText("Editor Settings");
                ImGui::Checkbox("Display performance metrics", &m_ShowMetrics);

                ImGui::SeparatorText("Global Prompt Settings");
                ImGui::Checkbox("Confirm component delete", &m_SkipDeleteComponentPrompt);
                ImGui::Checkbox("Confirm project save", &m_SkipOverwriteSavePrompt);
                ImGui::EndMenu();
            }

            // play-bar controls
            if (ImGui::Button("Play")) m_Play = true;
            if (ImGui::Button("Stop")) Stop();
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(255, 0, 0, 255));
            if (ImGui::Button("Reset")) ResetScene();
            ImGui::PopStyleColor();

            ImGui::EndMainMenuBar();
        }
    }
}