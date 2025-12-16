#include "Application.h"
#include "Renderer.h"
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

        m_Window.create(sf::VideoMode(m_Specs.m_WindowSize), m_Specs.m_WindowName);
        if (!m_Window.isOpen())
        {
            throw std::runtime_error("Failed to create application window.");
        }
        if(m_Specs.m_EditorEnabled)
        {
            if(!ImGui::SFML::Init(m_Window))
            {
                throw std::runtime_error("Failed to initialize imgui window.");
            }
        }
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    void Application::Run()
    {
        sf::RenderStates states;
        sf::Clock clock;

        m_SystemManager.InitializeStateSystem(m_SceneManager);
        
        Renderer Renderer(m_Window);

        while (m_IsRunning)
        {
            // Event polling
            m_Window.handleEvents
            (
                [&](const sf::Event::MouseButtonPressed& event) { if(m_Specs.m_EditorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::MouseButtonReleased& event) { if(m_Specs.m_EditorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::MouseMoved& event) { if(m_Specs.m_EditorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },
                [&](const sf::Event::TextEntered& event) { if(m_Specs.m_EditorEnabled) ImGui::SFML::ProcessEvent(m_Window, event); },

                [&](const sf::Event::KeyPressed& event)
                {
                    m_InputSystem.PushKeyPress(event);
                },

                [&](const sf::Event::KeyReleased& event)
                {
                    m_InputSystem.PushKeyRelease(event);
                },

                [&](const sf::Event::Closed& event)
                {
                    Application::Close();
                }
            );

            // Update systems
            sf::Time tick = clock.restart();

            if(m_Specs.m_EditorEnabled)
            {
                ImGui::SFML::Update(m_Window, tick);
                m_Editor.Run(m_EntityManager, m_SceneManager, m_SystemManager);
            }

            m_InputSystem.Update(tick, m_EntityManager);
            
            m_SystemManager.UpdateSystems(tick, m_EntityManager);
            m_SystemManager.UpdateState(tick, m_EntityManager);

            if(m_SystemManager.GetStateSystem()->IsQuitRequested())
            {
                Application::Close();
                break;
            }
            if(m_SystemManager.GetStateSystem()->IsSceneChangeRequested())
            {
                std::string newState = m_SystemManager.GetStateSystem()->ConsumeChangeRequest();
                m_SceneManager.Transition(newState, m_EntityManager, m_SystemManager);
            }
            
            // Render
            m_Window.clear();

            Renderer.Render(states, m_EntityManager);

            if(m_Specs.m_EditorEnabled)
            {
                ImGui::SFML::Render(m_Window);
            }

            m_Window.display();
        }
        if(m_Specs.m_EditorEnabled) ImGui::SFML::Shutdown();
        m_Window.close();
        return;
    }
}