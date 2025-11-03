#include "Application.h"

namespace Spoon {    
    
    Application::Application(AppSpecifications& specs)
        : m_Specs (specs)
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
                [&](const auto& event)
                {
                    if(event->is<sf::Event::Closed>())
                    {
                        m_Window.close();
                        Application::Close();
                    }
                    else
                    {
                        // EVENT HANDLERS
                        m_LayerStack.PushEvent(event);                        
                    }

                }
            );

            // BEGIN RENDERING LOOP
            
            m_Window.clear();

            for (Layer* layer : m_LayerStack)
            {
                for (auto entity : layer.GetEntities())
                {
                    m_Window.draw(entity.m_Sprite);
                }
            }

            m_Window.display();
        }
    }
}