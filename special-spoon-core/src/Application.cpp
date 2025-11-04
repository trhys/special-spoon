#include "Application.h"

namespace Spoon {    
    
    Application::Application(const AppSpecifications& specs)
        : m_Specs(specs)
    {
        m_Window.create(sf::VideoMode(m_Specs.m_WindowSize), m_Specs.m_WindowName);
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    void Application::Run()
    {
        while (m_IsRunning)
        {
            // CHECK FOR EVENTS
            m_Window.handleEvents
            (
                [&](const sf::Event::KeyPressed& keyPress)
                {
                    m_LayerStack.PushEvent(keyPress);
                }

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

            // BEGIN RENDERING LOOP
            
            m_Window.clear();

            for (Layer* layer : m_LayerStack)
            {
                for (auto entity : layer->GetEntities())
                {
                    entity.draw(m_Window);
                }
            }

            m_Window.display();
        }
    }
}