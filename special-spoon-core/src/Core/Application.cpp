#include "Application.h"
#include "Renderer.h"
#include "Utils/MemoryUtils.h"

namespace Spoon 
{        
    Application* Application::s_Instance = nullptr;

    Application::Application(const AppSpecifications& specs)
        : m_Specs(specs)
    {
        SS_INSTANCE_ASSERT(s_Instance)
        s_Instance = this;

        if(m_Specs.PhysicsEnabled)
        {
            #define SS_PHYSICS_ENABLED
        }
        
        #ifndef SS_CONSOLE_ONLY
            m_Window.create(sf::VideoMode(m_Specs.m_WindowSize), m_Specs.m_WindowName);
        #endif
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    void Application::Run()
    {
        #ifndef SS_CONSOLE_ONLY
            sf::RenderStates states;
            sf::Clock clock;

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
                        m_Window.close();
                        Application::Close();
                        }
                    }
                );

                // Update systems
                sf::Time tick = clock.restart();

                m_InputSystem.Update(tick, m_EntityManager);

                m_SystemManager.UpdateSystems(tick, m_EntityManager);

                // Render
                m_Window.clear();

                Renderer.Render(states, m_EntityManager);
                
                m_Window.display();
            }
        #endif
        
        #ifdef SS_CONSOLE_ONLY
            sf::Clock clock;
            while (m_IsRunning)
            {
                // Update systems
                sf::Time tick = clock.restart();

                m_InputSystem.Update(tick, m_EntityManager);

                m_SystemManager.UpdateSystems(tick, m_EntityManager);
            }
        #endif
    }
}