#include "Application.h"
#include "LayerStack.h"
#include "ResourceManager.h"
#include "MemoryUtils.h"


namespace Spoon {    
    
    Application* Application::s_Instance = nullptr;

    Application::Application(const AppSpecifications& specs)
        : m_Specs(specs)
    {
        SS_INSTANCE_ASSERT(s_Instance)
        s_Instance = this;
    }

    void Application::PushLayer(Layer* layer)
    {
        layer->GetRSM(&m_RSM);
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    void Application::Run()
    {
        m_Window.create(sf::VideoMode(m_Specs.m_WindowSize),m_Specs.m_WindowName);
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

            // UPDATE LAYER STACK
            sf::Time tick = clock.restart();
            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate(tick);
            }

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