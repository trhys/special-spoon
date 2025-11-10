#include "Application.h"
#include "LayerStack.h"
#include "ResourceManager.h"
#include "MemoryUtils.h"


namespace Spoon {    
    
    Application* Application::s_Instance = nullptr;

    Application::Application(const AppSpecifications& specs)//, std::unordered_map<std::string, std::filesystem::path> resources)
        : m_Specs(specs)
    {
        SS_INSTANCE_ASSERT(s_Instance)
        s_Instance = this;
        //m_RSM.Init(resources);
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach(&m_RSM);
    }

    void Application::LoadScenes()
    {
        for (Layer* layer : m_LayerStack)
        {
            layer->LoadScene();
        }
    }

    void Application::Close()
    {
        m_IsRunning = false;
    }

    void Application::Run()
    {
        m_Window.create(sf::VideoMode(m_Specs.m_WindowSize),m_Specs.m_WindowName);
        sf::Transform identity_transform = sf::Transform::Transform();
        
        while (m_IsRunning)
        {
            
            // CHECK FOR EVENTS
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

            // BEGIN RENDERING LOOP
            
            m_Window.clear();

            for (Layer* layer : m_LayerStack)
            {
                layer->DrawScene(m_Window, identity_transform);
            }

            m_Window.display();
        }
    }
}