#include "Application.h"
#include "ResourceManager/ResourceManager.h"
#include "Serialization/Serializer.h"

#include "Editor/Utils/Viewport.h"

#include "Utils/MemoryUtils.h"
#include "Utils/Macros.h"
#include "Utils/UserConfig.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon 
{        
    Application* Application::s_Instance = nullptr;
    UserConfig config = LoadUserConfig();

    // Forward declarations for registration functions
    void RegisterDefaultLoaders();
    void RegisterDefaultSystems();

    Application::Application(const AppSpecifications& specs)
        : m_Specs(specs)
    {
        // Make sure only one instance of Application exists
        SS_INSTANCE_ASSERT(s_Instance)
        s_Instance = this;

        // Create the application window
        m_Window.create(m_Specs.mode, m_Specs.windowName);
        if (!m_Window.isOpen())
        {
            throw std::runtime_error("Failed to create application window.");
        }

        // Initialize ImGui if in editor mode
        if(m_Specs.editorEnabled)
        {
            if(!ImGui::SFML::Init(m_Window))
            {
                throw std::runtime_error("Failed to initialize imgui window.");
            }
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 

            m_Viewport.target = sf::RenderTexture({1280, 720});
        }

        // Register built-in registries before loading any scenes
        ActionRegistry::Get().RegisterBuiltIns();
        RegisterDefaultLoaders();
        RegisterDefaultSystems();
    }

    void Application::Close()
    {
        if(m_Specs.editorEnabled)
        {
            const char* popup = "Close";
            if (!ImGui::IsPopupOpen(popup))
                ImGui::OpenPopup(popup);

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

            if(ImGui::BeginPopupModal(popup, &closePrompt, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Are you sure you want to exit?");
                ImGui::Separator();

                if (ImGui::Button("Save and Exit"))
                {
                    if (m_Editor.GetActiveScene())
                    {
                        SerializeScene(*m_Editor.GetActiveScene(), m_EntityManager, m_SystemManager);
                        SerializeManifest(m_SceneManager);
                        SaveUserConfig(config);
                        m_IsRunning = false;
                        closePrompt = false;
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    closePrompt = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                if (ImGui::Button("Exit Without Saving")) 
                { 
                    m_IsRunning = false;
                    closePrompt = false;
                    SaveUserConfig(config);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopStyleColor();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("All unsaved changes will be lost!");
                
                ImGui::EndPopup();
            }
        }
        else m_IsRunning = false;
    }

    void Application::Update(sf::Time tick)
    {
        m_InputSystem.Update(tick, m_EntityManager);
                
        m_SystemManager.UpdateSystems(tick, m_EntityManager);
        m_SystemManager.UpdateState(tick, m_EntityManager);

        StateSystem* stateSystem = m_SystemManager.GetStateSystem();
        
        if (!stateSystem)
        {
            throw std::runtime_error("Failed to get state system from system manager.");
        }

        // Handle quit request
        if (stateSystem->IsQuitRequested())
        {
            stateSystem->ConsumeQuitFlag();
            if (m_Specs.editorEnabled) m_Editor.Stop();
            else closePrompt = true;
        }
        
        // Handle scene change request
        if (stateSystem->IsSceneChangeRequested())
        {
            std::string newState = stateSystem->ConsumeChangeRequest();
            m_SceneManager.Transition(newState, m_EntityManager, m_SystemManager);
        }
    }

    void Application::Shutdown()
    {
        m_EntityManager.Shutdown();
        m_Editor.Shutdown();

        if (m_Specs.editorEnabled) ImGui::SFML::Shutdown(m_Window);

        m_Viewport.target = sf::RenderTexture();

        ResourceManager::Get().ClearAllResources(true);

        (void)m_Window.setActive(false);
        if (m_Window.isOpen()) m_Window.close();
    }

    void Application::Run()
    {
        sf::RenderStates states;
        sf::Clock clock;

        bool play = true;
        
        m_SystemManager.InitializeStateSystem();
        
        while (m_IsRunning)
        {
            // Clear action queue
            m_ActionQueue.Clear();
            
            // Event polling
            m_Window.handleEvents
            (
                [&](const sf::Event::MouseButtonPressed& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::MouseButtonReleased& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::MouseMoved& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::MouseWheelScrolled& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::TextEntered& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },

                [&](const sf::Event::KeyPressed& event) 
                { 
                    m_InputSystem.PushKeyPress(event);
                    if (m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event);
                },
                [&](const sf::Event::KeyReleased& event) 
                { 
                    m_InputSystem.PushKeyRelease(event);
                    if (m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event);
                },

                [&](const sf::Event::Closed& event) { closePrompt = true; }
            );

            // Update systems
            sf::Time tick = clock.restart();

            if (m_Specs.editorEnabled)
            {
                ImGui::SFML::Update(m_Window, tick);
                if (closePrompt)
                    Close();
                ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
                play = m_Editor.Play();
            }

            if (play) Application::Update(tick);
            else m_InputSystem.ClearEvents();

            // Rendering
            m_Window.clear();

            if (m_Specs.editorEnabled)
            {
                ImGui::Begin("Viewport");

                RenderViewport(m_Viewport);

                // Draw to viewport
                m_Viewport.target.clear();

                if (m_Editor.GetActiveScene() && !m_EntityManager.GetAllEntities().empty())
                    m_Renderer.Render(m_Viewport.target, states, m_EntityManager);
                else // Draw logo if no scene is loaded or scene is empty
                {
                    const auto& logoTex = ResourceManager::Get().GetResource<sf::Texture>("special-spoon-logo");
                    sf::Sprite logoSprite(logoTex);
                    sf::Vector2f logoCenter(logoTex.getSize().x / 2.0f, logoTex.getSize().y / 2.0f);
                    sf::Vector2f viewportCenter(m_Viewport.target.getSize().x / 2.0f, m_Viewport.target.getSize().y / 2.0f);
                    logoSprite.setOrigin(logoCenter);
                    logoSprite.setPosition(viewportCenter);
                    m_Viewport.target.draw(logoSprite);
                }
                m_Viewport.target.display();

                ImGui::Image(m_Viewport.target);

                // Raycast to grab entities on the viewport
                if (ImGui::IsItemHovered() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                {
                    static UUID selectedID;
                    ImVec2 viewportPos = ImGui::GetItemRectMin();
                    ImVec2 mousePos = ImGui::GetIO().MousePos;
                    sf::Vector2i relativeMouse(
                        static_cast<int>(mousePos.x - viewportPos.x),
                        static_cast<int>(mousePos.y - viewportPos.y)
                    );
                    sf::Vector2f worldMouse = m_Viewport.target.mapPixelToCoords(relativeMouse);
                    std::vector<UUID> entities = m_EntityManager.RayPick(worldMouse);
                    if (!entities.empty())
                    {
                        auto it = std::find(entities.begin(), entities.end(), selectedID);
                        if (it != entities.end() && std::next(it) != entities.end())
                            selectedID = *(std::next(it));
                        else
                            selectedID = entities.front();
                    }
                    m_Editor.PickEntity(selectedID, m_EntityManager);
                }

                ImGui::End();
                
                m_Editor.Run(tick, m_EntityManager, m_SceneManager, m_SystemManager);
                ImGui::SFML::Render(m_Window);
            }
            else if (!m_Specs.editorEnabled)
            {
                m_Renderer.Render(m_Window, states, m_EntityManager);
            }

            m_Window.display();
        }
        Shutdown();
    }
}