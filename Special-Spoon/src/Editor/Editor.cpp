#include "Editor.h"

#include "ECS/Components/World/TileMapComp.h"

#include "Utils/EditorSettings.h"
#include "Utils/Helpmarker.h"
#include "Utils/Windows.h"

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
                
		// draw main dashboard
        DrawMainMenuBar();
        DrawInspector(tick);

        // draw project menus
        DrawCreateProjectPrompt();
        DrawCreateProjectError();
        DrawLoadProject();
        DrawLoadProjectError();
        DrawConfigProject();
		DrawOverwriteProjectPrompt();
        DrawSaveProjectError();

        // entity menus (some called in inspector)
        DrawCreateEntity();
        DrawAddComponent();
        DrawDeleteComponent();
        DrawBlueprintsMenu();

        // scene menus
        DrawNewScene();
        DrawSaveScene();
        DrawLoadScene();
        DrawDeleteScenes();
        
        // system menus
        DrawSystemsMenu();

        // Resource menus
        DrawResourcesMenu();

        // shutdown prompt
        DrawShutdownPrompt();

        // Tools
        if (m_AnimationTool.IsOpen()) m_AnimationTool.Update(tick);
        if (m_TileMapTool.IsOpen()) m_TileMapTool.Update(tick);
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
        //SelectEntity(id, this, *m_Services.entityManager);
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

    void Editor::RequestQuit()
    {
        m_ShowQuitPrompt = true;
    }

    void Editor::DrawShutdownPrompt()
    {
        if (!m_ShowQuitPrompt)
            return;

        const char* popup = "Close";
        if (!ImGui::IsPopupOpen(popup))
            ImGui::OpenPopup(popup);

        CenterNext();
        if(ImGui::BeginPopupModal(popup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to exit?");
            ImGui::Separator();

            if (ImGui::Button("Save and Exit"))
            {
                if (GetActiveScene())
                {
                    SerializeScene(*GetActiveScene(), *m_Services.entityManager, *m_Services.systemManager);
                    SerializeManifest(*m_Services.sceneManager);
                    //SaveUserConfig(config);
                    m_ShowQuitPrompt = false;
                    Application::Get().FinalizeQuit();
                    ImGui::CloseCurrentPopup();
                } else {
                    m_ShowQuitPrompt = false;
                    Application::Get().FinalizeQuit();
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                m_ShowQuitPrompt = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Exit Without Saving")) 
            { 
                m_ShowQuitPrompt = false;
                //SaveUserConfig(config);
                Application::Get().FinalizeQuit();
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("All unsaved changes will be lost!");
            
            ImGui::EndPopup();
        }
    }
}
