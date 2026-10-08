#pragma once

#include "Renderable.h"
#include "WorldProjection.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/Project/Policies.h"
#include "ECS/ECS.h"
#include "Editor/Overlay.h"

#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Renderer
    {
    public:
        void Render(sf::RenderTarget& target, sf::RenderStates states, EntityManager& manager);
        void DepthSort();

        // Update render config
        void UpdateRenderConfig();

        // projection helpers
        bool IsProjectionEnabled() const { return UsesIsometricProjection(activeSortPolicy); }
        const WorldProjection& GetProjection() const { return m_Projection; }
        void UpdateProjection(EntityManager& manager);

        // Metrics
        int GetDrawCalls() const { return m_DrawCalls; }
        float GetDrawTime() const { return m_DrawTime; }

        // Editor overlay
        void PushOverlay(OverlayCmd cmd) { m_OverlayCommands.push_back(std::move(cmd)); }
        void ClearOverlay() { m_OverlayCommands.clear(); }

    private:
        // iterable renderables for the current frame
        std::vector<Renderable> m_Renderables;
        std::vector<OverlayCmd> m_OverlayCommands;

        // helper struct for managing projection state
        WorldProjection m_Projection;
        
        // project level config for active projection policy
        ActiveSortPolicy activeSortPolicy = ActiveSortPolicy::Isometric;

        // metrics
        int m_DrawCalls = 0;
        float m_DrawTime = 0.f;
    };
}
