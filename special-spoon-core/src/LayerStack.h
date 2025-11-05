#pragma once

#include "Layer.h"

#include <vector>

namespace Spoon {
    class LayerStack
    {
    public:
        LayerStack() {}
        ~LayerStack() {}

        void PushLayer(Layer* layer);
        void PushEvent(const sf::Event& event);

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
        std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

    private:
        std::vector<Layer*> m_Layers;
        int m_LayerIndex = 0;

    };
}