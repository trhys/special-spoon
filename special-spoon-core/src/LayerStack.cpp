#include "LayerStack.h"

namespace Spoon
{
    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.push_back(layer);
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
        layer->OnDetach();
        m_Layers.erase(it);
    }

    void LayerStack::PushEvent(const sf::Event& event)
    {
        bool Handled = false;
        auto it = LayerStack::rend();
        while(!Handled)
        {
            for (Layer* layer : m_Layers)
            {
                if(layer->OnEvent(event)) { break; }
            }
            Handled = true;
        }
    }
}