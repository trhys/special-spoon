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
                for (auto& id : manager.GetAllEntitiesWithComponent<ColliderComp>(ColliderComp::Name))
                {
                    if (!transformArray.m_IdToIndex.count(id))
                        continue;

                    auto& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
                    auto& collider = manager.GetComponent<ColliderComp>(id, ColliderComp::Name);
                    const sf::FloatRect bounds = collider.GetWorldBounds(transform.GetPosition());

                    if (collider.GetType() == ColliderType::AABB)
                    {
                        sf::RectangleShape rect(bounds.size);
                        rect.setPosition(bounds.position);
                        rect.setFillColor(sf::Color::Transparent);
                        rect.setOutlineColor(sf::Color::Yellow);
                        rect.setOutlineThickness(10.0f);
                        target.draw(rect, states);
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