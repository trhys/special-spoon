#pragma once

#include "EntityManager.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class RenderSystem
    {
    public:
        RenderSystem(sf::RenderTarget& target) : m_Target(target) {}
        ~RenderSystem() {}

        void Render(sf::RenderStates states, EntityManager& manager)
        {
            auto& transformArray = manager.GetArray<TransformComp>();
            auto& spriteArray = manager.GetArray<SpriteComp>();
            auto& textArray = manager.GetArray<TextComp>();
            auto& colorArray = manager.GetArray<ColorComp>();

            for(size_t in = 0; in < spriteArray.m_Components.size(); in++)
            {
                SpriteComp& sprite = spriteArray.m_Components[in];
                UUID id = spriteArray.m_IndexToId[in];

                if(transformArray.m_IdToIndex.count(id))
                {
                    TransformComp& transform = transformArray.m_Components[transformArray.m_IdToIndex[id]];
                    sprite.SetPosition(transform.GetPosition());
                }

                if(colorArray.m_IdToIndex.count(id)) 
                {
                    ColorComp& color = colorArray.m_Components[colorArray.m_IdToIndex[id]];
                    sprite.SetColor(color.m_Color);
                }

                m_Target.draw(sprite.m_Sprite, states);
            }

            for(size_t in = 0; in < textArray.m_Components.size(); in++)
            {
                TextComp& text = textArray.m_Components[in];
                UUID id = textArray.m_IdToIndex[id];

                if(transformArray.m_IdToIndex.count(id))
                {
                    TransformComp& transform = transformArray.m_Components[transformArray.m_IdToIndex[id]];
                    text.SetPosition(transform.GetPosition());
                }

                if(colorArray.m_IdToIndex.count(id)) 
                {
                    ColorComp& color = colorArray.m_Components[colorArray.m_IdToIndex[id]];
                    text.SetColor(color.m_Color);
                }

                m_Target.draw(text.m_Text, states);
            }
        }

    private:
        sf::RenderTarget& m_Target;
    };
}