#pragma once

#include "EntityManager.h"
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
            m_Renderables.clear();
            auto& layerArray = manager.GetArray<RenderLayer>();
            for(size_t index = 0; index < layerArray.m_Components.size(); index++)
            {
                RenderLayer& renderLayer = layerArray.m_Components[index];
                UUID ID = layerArray.m_IndexToId[index];

                m_Renderables.emplace_back(ID, renderLayer.m_Layer);
            }
            std::sort(m_Renderables.begin(), m_Renderables.end(),
                [](Renderable& a, Renderable& b){ return a.m_Layer < b.m_Layer; });

            auto& transformArray = manager.GetArray<TransformComp>();
            auto& spriteArray = manager.GetArray<SpriteComp>();
            auto& textArray = manager.GetArray<TextComp>();
            auto& colorArray = manager.GetArray<ColorComp>();

            for(auto& renderable : m_Renderables)
            {
                UUID ID = renderable.m_ID;
                if(spriteArray.m_IdToIndex.count(ID))
                {
                    SpriteComp& sprite = manager.GetComponent<SpriteComp>(ID);

                    if(transformArray.m_IdToIndex.count(ID))
                    {
                        TransformComp& transform = manager.GetComponent<TransformComp>(ID);
                        sprite.SetPosition(transform.GetPosition());
                        sprite.SetScale(transform.GetScale());
                    }

                    if(colorArray.m_IdToIndex.count(ID)) 
                    {
                        ColorComp& color = manager.GetComponent<ColorComp>(ID);
                        sprite.SetColor(color.m_Color);
                    }
                    target.draw(sprite.m_Sprite, states);
                }

                if(textArray.m_IdToIndex.count(ID))
                {
                    TextComp& text = manager.GetComponent<TextComp>(ID);

                    if(transformArray.m_IdToIndex.count(ID))
                    {
                        TransformComp& transform = manager.GetComponent<TransformComp>(ID);
                        text.SetPosition(transform.GetPosition());
                    }

                    if(colorArray.m_IdToIndex.count(ID)) 
                    {
                        ColorComp& color = manager.GetComponent<ColorComp>(ID);
                        text.SetColor(color.m_Color);
                    }
                    target.draw(text.m_Text, states);
                }
            }

            // Editor gizmos

            for (auto& comp : transformArray.m_Components)
            {
                if (comp.ActiveGizmo())
                {
                    target.draw(comp.rect, states);
                }
            }
        }

    private:
        std::vector<Renderable> m_Renderables;
    };
}