#include "Application.h"

namespace Spoon {    
    
    Application::Application()
    {
        // PLACEHOLDER
    }

    Layer* Application::CreateLayer()
    {
        return new Layer();
    }

    std::vector<Layer*> Application::GetLayerStack()
    {
        return LayerStack;
    } 
    
    void Application::PushLayer(Layer* layer)
    {
        LayerStack.emplace_back(layer);
    }

    void Application::Run()
    {
        LayerStack[0]->OnAttach(); // INIT TEST LAYER
        sf::CircleShape shape(100.f); ///////////////
        shape.setFillColor(sf::Color::Green); ///////

        while (m_IsRunning)
        {
            for (Layer* layer : LayerStack)
            {
                layer->GetWindow().clear();
                layer->GetWindow().draw(shape);
                layer->GetWindow().display();
            }
        }
    }
}