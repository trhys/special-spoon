#include "Application.h"
#include "ResourceManager/ResourceManager.h"
#include "Serialization/Serializer.h"

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
        : m_Specs(specs), 
        m_Editor(
            Services{ &m_Renderer, &m_EntityManager, &m_SystemManager, &m_SceneManager, &m_ProjectManager, &m_InputSystem }
        )
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
        
        // Handle scene change request
        if (stateSystem->IsSceneChangeRequested())
        {
            std::string newState = stateSystem->ConsumeChangeRequest();
            m_SceneManager.Transition(newState, m_EntityManager, m_SystemManager);
        }
    }

    void Application::HandleInput()
    {
        // Event polling -
        // Dispatches event to both the core input system and the editor (if enabled)
        m_Window.handleEvents
        (
            [&](const sf::Event::MouseButtonPressed& event) 
            { 
                if(m_Specs.editorEnabled) {
                    ImGui::SFML::ProcessEvent(m_Window, event); 
                }
                m_InputSystem.PushMouseButtonPress(event);
            },
            [&](const sf::Event::MouseButtonReleased& event) 
            { 
                if(m_Specs.editorEnabled) 
                {
                    ImGui::SFML::ProcessEvent(m_Window, event);
                }
                m_InputSystem.PushMouseButtonRelease(event);
            },
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
            [&](const sf::Event::FocusLost&)
            {
                m_InputSystem.ResetRuntimeState(m_EntityManager);
            },
            [&](const sf::Event::Closed& event) { HandleQuit(); }
        );
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

            // reap component cache
            m_EntityManager.ProcessReaper();
            
            // poll events and dispatch
            HandleInput();

            // Update systems
            sf::Time tick = clock.restart();

            if (m_Specs.editorEnabled)
            {
                ImGui::SFML::Update(m_Window, tick);
                ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
                play = m_Editor.Play();
            }

            if (play) Application::Update(tick);
            else m_InputSystem.ResetRuntimeState(m_EntityManager);

            // Rendering
            m_Window.clear();

            if (m_Specs.editorEnabled) // draw to viewport
            {
                // display and interaction with the viewport
                m_Editor.HandleViewport(m_Viewport, states);

                // run the editor logic
                m_Editor.Run(tick);

                // draw
                ImGui::SFML::Render(m_Window);
            }
            else // draw straight to window
                m_Renderer.Render(m_Window, states, m_EntityManager);

            m_Window.display();
        }
        Shutdown();
    }

    void Application::HandleQuit()
    {
        if (m_Specs.editorEnabled)
        {
            if (m_Editor.Play())
                m_Editor.Stop();
            else
                m_Editor.RequestQuit();
        }
        else m_IsRunning = false;
    }

    void Application::FinalizeQuit()
    {
        m_IsRunning = false;
    }

    void Application::Shutdown()
    {
        m_Editor.Shutdown();
        m_EntityManager.Shutdown();

        if (m_Specs.editorEnabled) ImGui::SFML::Shutdown(m_Window);

        m_Viewport.target = sf::RenderTexture();

        ResourceManager::Get().ClearAllResources(true);

        (void)m_Window.setActive(false);
        if (m_Window.isOpen()) m_Window.close();
    }
}