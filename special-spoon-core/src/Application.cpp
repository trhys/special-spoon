#include "Application.h"
#include "LayerStack.h"

#include "Physics/PhysicsManager.h"
#include "Scene/SceneManager.h"
#include "ResourceManager.h"

#include "MemoryUtils.h"


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

        m_Window.create(sf::VideoMode(m_Specs.m_WindowSize), m_Specs.m_WindowName);
    }

    void Application::PushLayer(Layer* layer)
    {
        layer->Init(&m_SceneManager, &m_ResourceManager);
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer* layer)
    {
        m_LayerStack.PopLayer(layer);
    }

    void Application::UpdatePhysics()
    {
        m_PhysicsManager.CheckCollision(m_SceneManager.GetActiveScene());
    }

    SceneManager* Application::GetSM() { return &m_SceneManager; }
    ResourceManager* Application::GetRM() { return &m_ResourceManager; }

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

            #define SS_PHYS_TEST // TEST QUADTREE AND COLLISION --- DRAWS QUADTREE NODES ON SCREEN FOR VISUAL REFERENCE
            #ifdef SS_PHYS_TEST
                for(auto& leaf : m_PhysicsManager.PhysTest())
                {
                    m_Window.draw(leaf);
                }
            #endif

            // RENDER
            m_Window.clear();

            m_SceneManager.DrawScene(m_Window, states);
            
            m_Window.display();
        }
    }
}