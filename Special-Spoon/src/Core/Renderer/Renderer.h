#pragma once

#include "Renderable.h"
#include "WorldProjection.h"
#include "Core/Project/Policies.h"
#include "ECS/ECS.h"

#include "SFML/Graphics.hpp"

namespace Spoon
{
    class EntityManager;
    class SystemManager;

    class Renderer
    {
    public:
        void Render(sf::RenderTarget& target, 
            sf::RenderStates states, 
            EntityManager& manager,
            const SystemManager& systemManager
        );
        void DepthSort();
        void BuildRenderables(EntityManager& manager);

        // Update render config
        void UpdateRenderConfig();

        // projection helpers
        bool IsProjectionEnabled() const { return UsesIsometricProjection(activeSortPolicy); }
        const WorldProjection& GetProjection() const { return m_Projection; }
        void UpdateProjection(EntityManager& manager);

        // Metrics
        int GetDrawCalls() const { return m_DrawCalls; }
        float GetDrawTime() const { return m_DrawTime; }

    private:
        // iterable renderables for the current frame
        std::vector<Renderable> m_WorldRenderables;
        std::vector<Renderable> m_ScreenRenderables;

        // helper struct for managing projection state
        WorldProjection m_Projection;
        
        // project level config for active projection policy
        ActiveSortPolicy activeSortPolicy = ActiveSortPolicy::Isometric;

        // metrics
        int m_DrawCalls = 0;
        float m_DrawTime = 0.f;
    };
}
