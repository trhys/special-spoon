#include "Overlay.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"

namespace Spoon
{
    void Overlay::PushOverlay()
    {
        if (selectedEntityOverlay)
        {
            Application::Get().GetRenderer().PushOverlay(selectedEntityOverlayCmd);
        }

        if (colliderOverlay)
        {
            Application::Get().GetRenderer().PushOverlay(colliderOverlayCmd);
        }
    }

    OverlayCmd Overlay::colliderOverlayCmd = OverlayCmd{ 
            .draw = [](sf::RenderTarget& target, sf::RenderStates states) 
            {
                auto& manager = Application::Get().GetEntityManager();
                auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
                const WorldProjection& projection = Application::Get().GetRenderer().GetProjection();
                for (auto& id : manager.GetAllEntitiesWithComponent<ColliderComp>(ColliderComp::Name))
                {
                    if (!transformArray.m_IdToIndex.count(id))
                        continue;

                    auto& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                    auto& collider = manager.GetComponent<ColliderComp>(id, ColliderComp::Name);
                    const sf::FloatRect bounds = collider.GetWorldBounds(transform.GetPosition());

                    // bounds are logical collider data. they are only projected
                    // for display - an AABB becomes a diamond/parallelogram on a
                    // projected map, but the solver still tests the logical AABB.
                    if (collider.GetType() == ColliderType::AABB)
                    {
                        const sf::Vector2f corners[4] = {
                            bounds.position,
                            { bounds.position.x + bounds.size.x, bounds.position.y },
                            bounds.position + bounds.size,
                            { bounds.position.x, bounds.position.y + bounds.size.y }
                        };

                        sf::ConvexShape shape(4);
                        for (std::size_t i = 0; i < 4; ++i)
                            shape.setPoint(i, projection.LogicalToScreen(corners[i]));
                        shape.setFillColor(sf::Color::Transparent);
                        shape.setOutlineColor(sf::Color::Yellow);
                        shape.setOutlineThickness(projection.IsActive() ? 2.0f : 10.0f);
                        target.draw(shape, states);
                    }
                }
            } 
        };

    OverlayCmd Overlay::selectedEntityOverlayCmd = OverlayCmd{
            .draw = [](sf::RenderTarget& target, sf::RenderStates states)
            {
                auto& editor = Application::Get().GetEditor();
                target.draw(editor.m_SelectionRect, states);
            }
        };
}