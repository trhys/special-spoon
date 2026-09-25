#include "Core/Renderer/Renderer.h"
#include "Core/Application.h"

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
					renderableComp->PreRender(manager, ID);
                    m_Renderables.emplace_back(Renderable{ ID, renderLayer.m_Layer, 0.0f, renderableComp });
                }
			}
        }
		
		DepthSort();
		
        std::sort(m_Renderables.begin(), m_Renderables.end(),
            [](Renderable& a, Renderable& b) { 
				return  std::tie(a.m_Layer, a.m_Depth, a.m_ID) <
       					std::tie(b.m_Layer, b.m_Depth, b.m_ID); 
			});

        for (auto& renderable : m_Renderables)
        {
			if (!renderable.m_Component)
				continue;
			
			renderable.m_Component->Render(target, states);
            m_DrawCalls++;
        }

        // Editor overlay
        for (auto& cmd : m_OverlayCommands)
        {
            cmd.draw(target, states);
            m_DrawCalls++;
        }
        ClearOverlay();

        // Return metrics
        m_DrawTime = static_cast<float>(drawClock.getElapsedTime().asMilliseconds());
    }

	void Renderer::DepthSort()
	{
		// decide what depth computation to use based on active policy
		switch (activeSortPolicy) {
			case ActiveSortPolicy::Isometric: {
				IsometricProjection policy;
				policy.ComputeDepth(m_Renderables); 
				break;
			}
		}
	}

	void Renderer::UpdateRenderConfig()
	{
		if (auto* project = Application::Get().GetProjectManager().GetCurrentProject())
		{
		    activeSortPolicy = project->config.SortPolicy;
		}	
	}
}
