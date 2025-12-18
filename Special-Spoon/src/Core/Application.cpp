#include "Application.h"
#include "ComponentLoaders.h"

#include "Utils/MemoryUtils.h"
#include "Utils/Macros.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon 
{        
    Application* Application::s_Instance = nullptr;

    Application::Application(const AppSpecifications& specs)
        : m_Specs(specs)
    {
        SS_INSTANCE_ASSERT(s_Instance)
        s_Instance = this;

        m_Window.create(m_Specs.mode, m_Specs.windowName);
        if (!m_Window.isOpen())
        {
            throw std::runtime_error("Failed to create application window.");
        }
        if(m_Specs.editorEnabled)
        {
            if(!ImGui::SFML::Init(m_Window))
            {
                throw std::runtime_error("Failed to initialize imgui window.");
            }
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 

            m_EditorViewport = sf::RenderTexture({1280, 720});
        }
        m_SceneManager.LoadManifest(m_Specs.sceneManifestPath);
    }

    void Application::Close()
    {
        if(m_Specs.editorEnabled)
        {
            ImGui::Begin("Close prompt");
            const char* popup = "Close";
            ImGui::OpenPopup(popup);

            if(ImGui::BeginPopupModal(popup))
            {
                ImGui::Text("Are you sure you want to exit?");
                ImGui::Separator();

                if(ImGui::Button("Yes")) 
                { 
                    m_IsRunning = false;
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("No")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            ImGui::End();
        }
        else m_IsRunning = false;
    }

    void Application::Update(sf::Time tick)
    {
        m_InputSystem.Update(tick, m_EntityManager);
                
        m_SystemManager.UpdateSystems(tick, m_EntityManager);
        m_SystemManager.UpdateState(tick, m_EntityManager);

        if(m_SystemManager.GetStateSystem()->IsQuitRequested())
        {
            if(m_Specs.editorEnabled)
            {
                m_Editor.Stop();
            }
            else Application::Close();
        }
        if(m_SystemManager.GetStateSystem()->IsSceneChangeRequested())
        {
            std::string newState = m_SystemManager.GetStateSystem()->ConsumeChangeRequest();
            m_SceneManager.Transition(newState, m_EntityManager, m_SystemManager);
        }
    }

    void Application::Run()
    {
        sf::RenderStates states;
        sf::Clock clock;

        bool play = true;
        
        ResourceManager::ScanAssets(m_Specs.workingDir);
        m_SystemManager.InitializeStateSystem(m_SceneManager);

        while (m_IsRunning)
        {
            // Event polling
            m_Window.handleEvents
            (
                [&](const sf::Event::MouseButtonPressed& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::MouseButtonReleased& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::MouseMoved& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::TextEntered& event) { if(m_Specs.editorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },

                [&](const sf::Event::KeyPressed& event) { m_InputSystem.PushKeyPress(event); },
                [&](const sf::Event::KeyReleased& event) { m_InputSystem.PushKeyRelease(event); },

                [&](const sf::Event::Closed& event) { Application::Close(); }
            );

            // Update systems
            sf::Time tick = clock.restart();

            if(m_Specs.editorEnabled)
            {
                ImGui::SFML::Update(m_Window, tick);
                ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
                play = m_Editor.Play();
            }

            if(play) Application::Update(tick);
            else m_InputSystem.ClearEvents();

            // Rendering
            if(m_Specs.editorEnabled && !play)
            {
                ImGui::Begin("Viewport");

                // Resize viewport if necessary
                sf::Vector2f viewportSize = ImGui::GetContentRegionAvail();
                sf::Vector2u viewport2u(
                    std::max(1u, static_cast<unsigned int>(viewportSize.x)),
                    std::max(1u, static_cast<unsigned int>(viewportSize.y)));
                if(m_EditorViewport.getSize() != viewport2u)
                    if(m_EditorViewport.resize({viewport2u}))
                    {
                        sf::View view(sf::FloatRect({0.f, 0.f}, { (float)viewport2u.x, (float)viewport2u.y }));
                        m_EditorViewport.setView(view);
                    }
                
                // Draw to viewport
                m_EditorViewport.clear();
                m_Renderer.Render(m_EditorViewport, states, m_EntityManager);
                m_EditorViewport.display();

                ImGui::Image(m_EditorViewport);
                ImGui::End();
                m_Editor.Run(m_EntityManager, m_SceneManager, m_SystemManager);

                m_Window.clear();
                ImGui::SFML::Render(m_Window);
                m_Window.display();
            }
            else
            {
                m_Window.clear();
                m_Renderer.Render(m_Window, states, m_EntityManager);
                m_Window.display();
            }
        }
        if(m_Specs.editorEnabled) ImGui::SFML::Shutdown();
        m_Window.close();
        return;
    }
}