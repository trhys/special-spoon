#include "LayerStack.h"

namespace Spoon
{
    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerIndex, layer);
        m_LayerIndex++;
    }

    void LayerStack::PushEvent(const sf::Event& event)
    {
        bool Handled = false;
        auto it = LayerStack::rend();
        while(!Handled)
        {
            if(m_Layers[it]->OnEvent(event)) { break; }
            else if(it = LayerStack::begin()) { break; }
            else { it--; }
        }
    }
}