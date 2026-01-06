#pragma once

#include "EntityManager/EntityManager.h"
#include "ECS/ECS.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    struct Renderable
    {
        Renderable(UUID id, int layer) : m_ID(id), m_Layer(layer) {}
        UUID m_ID;
        int m_Layer;
    };

    class Renderer
    {
    public:
        Renderer() {}
        ~Renderer() {}

        void Render(sf::RenderTarget& target, sf::RenderStates states, EntityManager& manager)
        {
            // Renderer metrics
            m_DrawCalls = 0;
            m_DrawTime = 0.f;
            sf::Clock drawClock;

            // Begin sorting renderables by layer
            m_Renderables.clear();
            auto& layerArray = manager.GetArray<RenderLayer>(RenderLayer::Name);
            for(size_t index = 0; index < layerArray.m_Components.size(); index++)
            {
                RenderLayer& renderLayer = layerArray.m_Components[index];
                UUID ID = layerArray.m_IndexToId[index];

                m_Renderables.emplace_back(ID, renderLayer.m_Layer);
            }
            std::sort(m_Renderables.begin(), m_Renderables.end(),
                [](Renderable& a, Renderable& b){ return a.m_Layer < b.m_Layer; });

            // Get component arrays
            auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
            auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
            auto& textArray = manager.GetArray<TextComp>(TextComp::Name);
            auto& colorArray = manager.GetArray<ColorComp>(ColorComp::Name);

            for(auto& renderable : m_Renderables)
            {
                UUID ID = renderable.m_ID;

                // Draw sprite component if it exists
                if(spriteArray.m_IdToIndex.count(ID))
                {
                    SpriteComp& sprite = manager.GetComponent<SpriteComp>(ID);

                    if(transformArray.m_IdToIndex.count(ID))
                    {
                        TransformComp& transform = manager.GetComponent<TransformComp>(ID);
                        sprite.SetPosition(transform.GetPosition());
                        sprite.SetScale(transform.GetScale());
                        sprite.SetRotation(transform.m_Transform.getRotation().asDegrees());
                    }

                    if(colorArray.m_IdToIndex.count(ID)) 
                    {
                        ColorComp& color = manager.GetComponent<ColorComp>(ID);
                        sprite.SetColor(color.m_Color);
                    }
                    target.draw(sprite.m_Sprite, states);
                    m_DrawCalls++;
                }

                // Draw text component if it exists
                if(textArray.m_IdToIndex.count(ID))
                {
                    TextComp& text = manager.GetComponent<TextComp>(ID);

                    if(transformArray.m_IdToIndex.count(ID))
                    {
                        TransformComp& transform = manager.GetComponent<TransformComp>(ID);
                        text.SetPosition(transform.GetPosition());
                        text.SetRotation(transform.m_Transform.getRotation().asDegrees());
                    }

                    if(colorArray.m_IdToIndex.count(ID)) 
                    {
                        ColorComp& color = manager.GetComponent<ColorComp>(ID);
                        text.SetColor(color.m_Color);
                    }
                    target.draw(text.m_Text, states);
                    m_DrawCalls++;
                }
            }

            // Editor gizmos
            for (auto& comp : transformArray.m_Components)
            {
                if (comp.ActiveGizmo())
                {
                    target.draw(comp.rect, states);
                    m_DrawCalls++;
                }
            }

            // Return metrics
            m_DrawTime = static_cast<float>(drawClock.getElapsedTime().asMilliseconds());
        }

        int GetDrawCalls() const { return m_DrawCalls; }
        float GetDrawTime() const { return m_DrawTime; }

    private:
        std::vector<Renderable> m_Renderables;
        int m_DrawCalls = 0;
        float m_DrawTime = 0.f;
    };
}