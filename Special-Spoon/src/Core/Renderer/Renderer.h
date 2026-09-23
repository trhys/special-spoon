#pragma once

#include "Renderable.h"
#include "Gizmo.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/Project/Policies.h"
#include "ECS/ECS.h"
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

        // Metrics
        int GetDrawCalls() const { return m_DrawCalls; }
        float GetDrawTime() const { return m_DrawTime; }

        // Editor gizmos
        void AddActiveGizmo(GizmoCommand cmd) { m_Gizmos.push_back(std::move(cmd)); }
        void ClearActiveGizmos() { m_Gizmos.clear(); }

    private:
        void DrawColliderOverlay(sf::RenderTarget& target, sf::RenderStates states, EntityManager& manager);

        std::vector<Renderable> m_Renderables;
        std::vector<GizmoCommand> m_Gizmos;
        ActiveSortPolicy activeSortPolicy = ActiveSortPolicy::Isometric;
        int m_DrawCalls = 0;
        float m_DrawTime = 0.f;
    };
}
