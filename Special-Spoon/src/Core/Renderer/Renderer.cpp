#include "Core/Renderer/Renderer.h"
#include "Core/Application.h"

namespace Spoon {
    void Renderer::Render(sf::RenderTarget& target, sf::RenderStates states, EntityManager& manager)
    {
        // Renderer metrics
        m_DrawCalls = 0;
        m_DrawTime = 0.f;
        sf::Clock drawClock;

        UpdateProjection(manager);
        BuildRenderables(manager);	
		DepthSort();
		
        std::sort(m_WorldRenderables.begin(), m_WorldRenderables.end(),
            [](Renderable& a, Renderable& b) { 
				return  std::tie(a.m_Layer, a.m_Depth, a.m_ID) <
       					std::tie(b.m_Layer, b.m_Depth, b.m_ID); 
			});

        for (auto& renderable : m_WorldRenderables)
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
				policy.ComputeDepth(m_WorldRenderables); 
				break;
			}
			case ActiveSortPolicy::TopDown: {
				TopDownProjection policy;
				policy.ComputeDepth(m_WorldRenderables);
				break;
			}
		}
	}

	void Renderer::BuildRenderables(EntityManager& manager)
	{
		m_WorldRenderables.clear();
		m_ScreenRenderables.clear();

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
					bool isScreenSpace = false;

					// check for ui comp and determine what bucket it belongs in
					if (manager.HasComponent<UIComp>(ID, UIComp::Name))
					{
						auto& uiComp = manager.GetComponent<UIComp>(ID, UIComp::Name);
						isScreenSpace = uiComp.IsScreenSpace();
					}
					renderableComp->PreRender(manager, ID);

					if (isScreenSpace)
						m_ScreenRenderables.emplace_back(Renderable{ ID, renderLayer.m_Layer, 0.0f, renderableComp });
					else 
					{
						renderableComp->SetProjection(m_Projection);
						m_WorldRenderables.emplace_back(Renderable{ ID, renderLayer.m_Layer, 0.0f, renderableComp });
					}
                }
			}
        }
	}

	void Renderer::UpdateProjection(EntityManager& manager)
	{
		// rebuild projection each from from active tilemap
		// the tile map provides helpers for aqcuiring it's
		// logical dims separate from its projected ones
		m_Projection = WorldProjection{};
		auto& tileMaps = manager.GetArray<TileMapComp>(TileMapComp::Name);
		if (!tileMaps.m_Components.empty())
			m_Projection = tileMaps.m_Components.front().GetProjection(IsProjectionEnabled());
	}

	void Renderer::UpdateRenderConfig()
	{
		if (auto* project = Application::Get().GetProjectManager().GetCurrentProject())
		{
		    activeSortPolicy = project->config.SortPolicy;
		}	
	}
}
