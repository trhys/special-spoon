#include "Editor.h"

#include "Menus/SceneMenus.h"
#include "Menus/EntityMenus.h"
#include "Menus/SystemsMenu.h"
#include "Menus/ResourceMenus.h"

#include "Utils/EditorSettings.h"
#include "Utils/Helpmarker.h"

#include "Core/EntityManager.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "Core/SceneManager.h"
#include "Core/Serialization/Serializer.h"

#include "System/SystemManager.h"

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

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scene Manager"))
            {
                if (ImGui::MenuItem("New")) NewScene = true;
                if (ImGui::MenuItem("Load")) LoadScene = true;
                if (ImGui::MenuItem("Save")) Serialize(*m_ActiveScene, e_Manager, sys_Manager);
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

            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::Checkbox("Confirm component delete", &EditorSettings::Get().compDelAskAgain);
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

        ImGui::Separator();
        // todo 
        ImGui::End();

        // Scene menus
        if (NewScene)           { NewSceneMenu(s_Manager, this); }
        if (LoadScene)          { LoadSceneMenu(e_Manager, s_Manager, sys_Manager, this); }
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
}