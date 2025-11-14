#include "Application.h"
#include "LayerStack.h"
#include "Layer.h"
#include "Physics/PhysicsManager.h"
#include "ResourceManager.h"
#include "MemoryUtils.h"


namespace Spoon {    
    
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

        m_Window.create(sf::VideoMode(m_Specs.m_WindowSize), m_Specs.m_WindowName);
    }

    void Application::PushLayer(Layer* layer)
    {
        layer->Init(&m_RSM, &m_PM);
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::UpdatePhysics()
    {
        for (Layer* layer : m_LayerStack)
        {
            layer->Physics();
        }
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    void Application::Run()
    {
        sf::RenderStates states = sf::RenderStates::RenderStates();
        sf::Clock clock;

        while (m_IsRunning)
        {
            // EVENT HANDLING
            m_Window.handleEvents
            (
                [&](const sf::Event::KeyPressed& keyPress)
                {
                   m_LayerStack.PushEvent(keyPress);
                },

                [&](const auto& event)
                {
                    using T = std::decay_t<decltype(event)>;

                    if constexpr (std::is_same_v<T, sf::Event::Closed>)
                    {
                       m_Window.close();
                       Application::Close();
                    }
                    else
                    {
                       m_LayerStack.PushEvent(event);                        
                    }
                }
            );

            // UPDATE
            sf::Time tick = clock.restart();
            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate(tick);
            }

            // PHYSICS
            #ifdef SS_PHYSICS_ENABLED
                UpdatePhysics();
            #endif
            
            // RENDER
            m_Window.clear();

            for (Layer* layer : m_LayerStack)
            {
                layer->DrawScene(m_Window, states);
            }
            
            m_Window.display();
        }
    }
}