#include "Core/Renderer/Renderer.h"

namespace Spoon {
    void Renderer::Render(sf::RenderTarget& target, sf::RenderStates states, EntityManager& manager)
    {
        // Renderer metrics
        m_DrawCalls = 0;
        m_DrawTime = 0.f;
        sf::Clock drawClock;

        // Begin sorting renderables by layer
        m_Renderables.clear();
        auto& layerArray = manager.GetArray<RenderLayer>(RenderLayer::Name);
        for (size_t index = 0; index < layerArray.m_Components.size(); index++)
        {
            RenderLayer& renderLayer = layerArray.m_Components[index];
            UUID ID = layerArray.m_IndexToId[index];

			// cast where the entity has a renderable component
			// this comsumes the renderable interface and allows for custom rendering logic
            for (Component* comp : manager.GetAllComponentsOfEntity(ID))
            {
                if (IRenderable* renderableComp = dynamic_cast<IRenderable*>(comp))
                {
                    m_Renderables.emplace_back(Renderable{ ID, renderLayer.m_Layer, renderableComp });
                }
			}
        }
        std::sort(m_Renderables.begin(), m_Renderables.end(),
            [](Renderable& a, Renderable& b) { return a.m_Layer < b.m_Layer; });

        for (auto& renderable : m_Renderables)
        {
			if (!renderable.m_Component)
				continue;
			
			renderable.m_Component->PreRender(manager, renderable.m_ID);
			renderable.m_Component->Render(target, states);
            m_DrawCalls++;
        }

        // Editor gizmos
        for (auto& gizmo : m_Gizmos)
        {
            gizmo.draw(target, states);
            m_DrawCalls++;
        }
        ClearActiveGizmos();

        // Return metrics
        m_DrawTime = static_cast<float>(drawClock.getElapsedTime().asMilliseconds());
    }
}
