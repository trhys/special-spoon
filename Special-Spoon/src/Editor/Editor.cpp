#include "Editor.h"

#include "Menus/SceneMenus.h"
#include "Menus/EntityMenus.h"
#include "Menus/SystemsMenu.h"
#include "Menus/ResourceMenus.h"

#include "Utils/EditorSettings.h"
#include "Utils/Helpmarker.h"

#include "Core/Application.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "Core/Serialization/Serializer.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    void Editor::Stop() 
    {
        m_Play = false;
    }

    bool Editor::Play()
    {
        return m_Play;
    }

    void Editor::Run(sf::Time tick, EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
        if (!workingDir)
            workingDir = ResourceManager::Get().GetAssetsDir();

        if (!m_CurrentProject && !ImGui::IsPopupOpen("No Project Loaded"))
            ImGui::OpenPopup("No Project Loaded");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("No Project Loaded", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("No project is currently loaded. Please create or open a project to continue.");
            if (ImGui::Button("Create"))
            {
                NewProject = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Open"))
            {
                OpenProject = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project")) NewProject = true;
                if (ImGui::MenuItem("Open Project")) OpenProject = true;
                if (ImGui::MenuItem("Save Project", nullptr, false, m_CurrentProject != nullptr)) SaveProject = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) Application::Get().Close();
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
                if (ImGui::MenuItem("Entity View")) ViewEntities = !ViewEntities;
                if (ImGui::MenuItem("Entity Blueprints")) ImGui::SetTooltip("todo");
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
                ImGui::Checkbox("Display performance metrics", &EditorSettings::Get().displayEditorMetrics);
                ImGui::Checkbox("Display ImGui metrics", &EditorSettings::Get().displayImGuiMetrics);

                ImGui::SeparatorText("Global Prompt Settings");
                ImGui::Checkbox("Confirm component delete", &EditorSettings::Get().skipAskBeforeDeleteComp);
                ImGui::Checkbox("Confirm project save", &EditorSettings::Get().skipProjectSavePrompt);
                ImGui::EndMenu();
            }

            if (ImGui::Button("Play")) 
                m_Play = true;
            if (ImGui::Button("Stop"))
                Stop();

            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Special-Spoon Editor");
        if (m_ActiveScene)
        {
            ImGui::Text("Active Scene ID: %s", m_ActiveScene->ID.c_str());
            if (ViewEntities) ViewEntitiesMenu(e_Manager);
        }
        else
        {
            ImGui::Text("Load/Create a scene to view and create entities!");
        }

        // Displays metrics and debug info
        if (EditorSettings::Get().displayEditorMetrics)
        {
            ImGui::SeparatorText("Performance Metrics");
            ImGui::Text("AVG FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Current FPS: %.1f", 1.f / tick.asSeconds());
            ImGui::Text("Draw Calls: %d", Application::Get().GetRenderer().GetDrawCalls());
            ImGui::SameLine(); HelpMarker("Number of draw calls submitted to the GPU per frame.\n This only counts draws called from the Renderer.\n"
                                          "External draws (e.g. ImGui) are not counted.");
            ImGui::Text("Draw Time: %.2f ms", Application::Get().GetRenderer().GetDrawTime());
            ImGui::SameLine(); HelpMarker("Time spent in the Renderer drawing all renderables this frame.");
        }
        else ImGui::Separator();
        ImGui::SeparatorText("ImGui Metrics");
        if (EditorSettings::Get().displayImGuiMetrics)
            ImGui::ShowMetricsWindow(&EditorSettings::Get().displayImGuiMetrics);

        ImGui::End();

        // Project menus
        if (NewProject)         { Application::Get().GetProjectManager().CreateNew(this); }
        if (OpenProject)        { Application::Get().GetProjectManager().LoadProject(this); }
        if (SaveProject)        { Application::Get().GetProjectManager().SaveProject(this); }

        // Scene menus
        if (NewScene)           { NewSceneMenu(s_Manager, this); }
        if (LoadScene)          { LoadSceneMenu(e_Manager, s_Manager, sys_Manager, this); }
        if (SaveScene)          { SaveSceneMenu(e_Manager, sys_Manager, this); }
        if (ViewSceneManifest)  { SceneManifestMenu(s_Manager, this); }

        // System menu
        if (ViewSystemsMenu)    { SystemsMenu(sys_Manager, this); }

        // Resource menus
        if (ViewResources)      { ViewResourcesMenu(this); }
        if (LoadResources)      { LoadResourcesMenu(this); }

        // Tools
        if (m_AnimationTool.IsOpen()) m_AnimationTool.Update(tick);
    }

    void Editor::EditTextureRect(SpriteComp& comp)
    {
        m_TextureRectTool.Run(comp);
    }

    void Editor::PickEntity(UUID id, EntityManager& e_Manager)
    {
        SelectEntity(id, this, e_Manager);
    }

    void Editor::Shutdown()
    {
        m_AnimationTool.Shutdown();
        m_TextureRectTool.Shutdown();
    }
}