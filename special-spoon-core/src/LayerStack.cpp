#include "LayerStack.h"

namespace Spoon
{
    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerIndex, layer);
        m_LayerIndex++;
    }

    void LayerStack::PushEvent(sf::Event& event);
    {
        auto index = LayerStack::rend();
        while (index > 0)
        {
            m_Layers[index]->OnEvent(event);
            index--;
        }
    }
}