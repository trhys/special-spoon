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

        // Projection - gated by the project config's Projection Style. The
        // active projection maps logical space to presentation space for this
        // frame, using the scene's tilemap grid (identity when there is none).
        // See WorldProjection.h for the coordinate-space contract.
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
        std::vector<Renderable> m_Renderables;
        std::vector<OverlayCmd> m_OverlayCommands;
        WorldProjection m_Projection;
        
        ActiveSortPolicy activeSortPolicy = ActiveSortPolicy::Isometric;

        int m_DrawCalls = 0;
        float m_DrawTime = 0.f;
    };
}
