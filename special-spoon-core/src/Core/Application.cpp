#include "Application.h"
#include "Renderer.h"
#include "Utils/MemoryUtils.h"
#include "ComponentLoaders.h"

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
                [&](const sf::Event::KeyPressed& keyPress)
                {
                    m_InputSystem.PushKeyPress(keyPress);
                },

                [&](const auto& event)
                {
                    using T = std::decay_t<decltype(event)>;
                    if constexpr (std::is_same_v<T, sf::Event::Closed>)
                    {
                        Application::Close();
                    }
                }
            );

            // Update systems
            sf::Time tick = clock.restart();

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
            
            m_Window.display();
        }
        m_Window.close();
        return;
    }
}