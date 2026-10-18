#include "Editor.h"
#include "Core/Renderer/Renderer.h"
#include "System/Core/InputSystem.h"
#include "ECS/Components/World/TileMapComp.h"

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
    Editor::Editor(Services services)
        : m_Services(services) {}

    void Editor::HandleViewport(
        Viewport& viewport, 
        sf::RenderStates& states
    )
    {
        ImGui::Begin("Viewport");

        RenderViewport(viewport);
        viewport.target.clear();

        // build overlay
        m_Services.renderer->ClearOverlay();
        BuildOverlay();

        if (GetActiveScene() && !m_Services.entityManager->GetAllEntities().empty())
            m_Services.renderer->Render(viewport.target, states, *m_Services.entityManager);
        else // Draw logo if no scene is loaded or scene is empty
        {
            const auto& logoTex = ResourceManager::Get().GetResource<sf::Texture>("special-spoon-logo");
            sf::Sprite logoSprite(logoTex);
            sf::Vector2f logoCenter(logoTex.getSize().x / 2.0f, logoTex.getSize().y / 2.0f);
            sf::Vector2f viewportCenter(viewport.target.getSize().x / 2.0f, viewport.target.getSize().y / 2.0f);
            logoSprite.setOrigin(logoCenter);
            logoSprite.setPosition(viewportCenter);
            viewport.target.draw(logoSprite);
        }
        viewport.target.display();

        ImGui::Image(viewport.target);
        const bool viewportHovered = ImGui::IsItemHovered();
        const ImVec2 viewportImageMin = ImGui::GetItemRectMin();
        const ImVec2 viewportImageMax = ImGui::GetItemRectMax();

        // set input systems viewport so input knows 
        // if we are clicking in the viewport or not
        m_Services.inputSystem->SetViewport(
            &viewport.target,
            sf::FloatRect(
                {viewportImageMin.x, viewportImageMin.y},
                {viewportImageMax.x, viewportImageMax.y}
            ),
            viewportHovered
        );

        const bool viewportConsumed = HandleViewportTools(
            viewport,
            viewportHovered,
            viewportImageMin,
            viewportImageMax
        );

        // Raycast to grab entities on the viewport
        static bool rayPickFlag = false;
        static bool draggingEntity = false;
        static UUID dragEntity;
        static sf::Vector2f lastDragMouse;
        static UUID selectedID;

        if (!viewportConsumed &&
            viewportHovered &&
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
            !rayPickFlag)
        {
            rayPickFlag = true;
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            sf::Vector2i relativeMouse(
                static_cast<int>(mousePos.x - viewportImageMin.x),
                static_cast<int>(mousePos.y - viewportImageMin.y)
            );
            sf::Vector2f worldMouse = viewport.target.mapPixelToCoords(relativeMouse);
            std::vector<UUID> entities = m_Services.entityManager->RayPick(worldMouse);
            if (!entities.empty())
            {
                // pick from entities starting at the top
                auto it = std::find(entities.begin(), entities.end(), selectedID);
                if (it != entities.end() && std::next(it) != entities.end())
                    selectedID = *(std::next(it));
                else
                    selectedID = entities.front();
            }
            PickEntity(selectedID);

            // begin dragging the picked entity while not simulating
            auto& transformArray = m_Services.entityManager->GetArray<TransformComp>(TransformComp::Name);
            draggingEntity = !m_Play && !entities.empty() && transformArray.HasEntity(selectedID);
            if (draggingEntity)
            {
                dragEntity = selectedID;
                lastDragMouse = worldMouse;
                m_Services.entityManager->GetComponent<TransformComp>(dragEntity, TransformComp::Name)
                    .MoveTransform(worldMouse, true);
            }
        }
        else if (draggingEntity &&
            !viewportConsumed &&
            !m_Play &&
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            // mouse is presentation space - MoveTransform inverse
            // projects it so the stored transform stays logical
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            sf::Vector2i relativeMouse(
                static_cast<int>(mousePos.x - viewportImageMin.x),
                static_cast<int>(mousePos.y - viewportImageMin.y)
            );
            sf::Vector2f worldMouse = viewport.target.mapPixelToCoords(relativeMouse);
            auto& transformArray = m_Services.entityManager->GetArray<TransformComp>(TransformComp::Name);
            if (!transformArray.HasEntity(dragEntity))
                draggingEntity = false;
            else if (worldMouse != lastDragMouse)
            {
                lastDragMouse = worldMouse;
                m_Services.entityManager->GetComponent<TransformComp>(dragEntity, TransformComp::Name)
                    .MoveTransform(worldMouse, false);
                PickEntity(dragEntity);
            }
        }
        else {
            // Reset dragging and ray picking state when 
            // no entity is being interacted with
            draggingEntity = false;
            rayPickFlag = false;
        }
        ImGui::End();
    }

    void Editor::Stop() 
    {
        m_Play = false;
    }

    bool Editor::Play()
    {
        return m_Play;
    }

    void Editor::ResetScene()
    {
        if (!m_ActiveScene)
            return;

        m_Play = false;
        SetActiveScene(m_Services.sceneManager->LoadScene(
            m_ActiveScene->ID, *m_Services.entityManager, *m_Services.systemManager)
        );
    }

    void Editor::Run(sf::Time tick)
    {
        if (!workingDir)
            workingDir = ResourceManager::Get().GetAssetsDir();

        if (!m_CurrentProject && m_ShowNoProjectLoaded)
            NoProjectLoaded();
                
		// draw main menu bar
        DrawMainMenuBar();

        //  draw project menus
        DrawCreateProjectPrompt();
        DrawCreateProjectError();
        DrawLoadProject();
        DrawLoadProjectError();
        DrawConfigProject();
		DrawOverwriteProjectPrompt();
        DrawSaveProjectError();

        ImGui::Begin("Special-Spoon Editor");
        if (m_ActiveScene)
        {
            ImGui::Text("Active Scene ID: %s", m_ActiveScene->ID.c_str());
            if (ViewEntities) ViewEntitiesMenu(*m_Services.entityManager);
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

        // Scene menus
        if (NewScene)           { NewSceneMenu(*m_Services.sceneManager, this); }
        if (LoadScene)          { LoadSceneMenu(*m_Services.entityManager, *m_Services.sceneManager, *m_Services.systemManager, this); }
        if (SaveScene)          { SaveSceneMenu(*m_Services.entityManager, *m_Services.systemManager, this); }
        if (ViewSceneManifest)  { SceneManifestMenu(*m_Services.sceneManager, this); }

        // System menu
        if (ViewSystemsMenu)    { SystemsMenu(*m_Services.systemManager, this); }

        // Resource menus
        if (ViewResources)      { ViewResourcesMenu(this); }
        if (LoadResources)      { LoadResourcesMenu(this); }

        // Tools
        if (m_AnimationTool.IsOpen()) m_AnimationTool.Update(tick);
        if (m_TileMapTool.IsOpen()) m_TileMapTool.Update(tick);

        //Blueprints
        if (OpenBlueprintsMenu)     { BlueprintsMenu(*m_Services.entityManager); }
    }

    void Editor::EditTextureRect(bool& editing, sf::Texture& texture, sf::IntRect& rect)
    {
        editing = m_TextureRectTool.Run(texture, rect);
    }

    void Editor::EditTileMap(TileMapComp* comp)
    {
        m_TileMapTool.Open(comp);
    }

    void Editor::SetActiveScene(SceneData* scene)
    {
        if (m_ActiveScene != scene)
            m_TileMapTool.Close();

        m_ActiveScene = scene;
    }

    void Editor::PickEntity(UUID id)
    {
        SelectEntity(id, this, *m_Services.entityManager);
    }

    bool Editor::HandleViewportTools(
        Viewport& viewport,
        bool viewportHovered,
        const ImVec2& imageMin,
        const ImVec2& imageMax
    )
    {
        if (!m_TileMapTool.IsOpen())
            return false;

        return m_TileMapTool.HandleViewport(
            viewport,
            viewportHovered,
            imageMin,
            imageMax
        );
    }

    void Editor::Shutdown()
    {
        m_AnimationTool.Shutdown();
        m_TextureRectTool.Shutdown();
        m_TileMapTool.Close();
    }

    void Editor::BuildOverlay()
    {
        if (!m_ActiveScene)
            return;

        m_Overlay.PushOverlay();
    }

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
                if (ImGui::MenuItem("Entity Blueprints")) OpenBlueprintsMenu = !OpenBlueprintsMenu;
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

    bool Editor::SetCurrentProject(Project* project)
    {
        if (!project)
            return false;
        SetActiveScene(nullptr);
        ResourceManager::Get().ScanAssets(project->assetsPath);
        m_Services.sceneManager->LoadManifest(project->dataPath);
        m_Services.renderer->UpdateRenderConfig();
        m_CurrentProject = project;
        m_ShowCreateProjectPrompt = false;
        return true;
    }
}
