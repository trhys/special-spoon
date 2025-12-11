#pragma once

#include "EntityManager.h"
#include "ECS/ECS.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Renderer
    {
    public:
        Renderer(sf::RenderWindow& target) : m_Target(target) {}
        ~Renderer() {}

        void Render(sf::RenderStates states, EntityManager& manager)
        {
            auto& transformArray = manager.GetArray<TransformComp>();
            auto& spriteArray = manager.GetArray<SpriteComp>();
            auto& textArray = manager.GetArray<TextComp>();
            auto& colorArray = manager.GetArray<ColorComp>();

            for(size_t in = 0; in < spriteArray.m_Components.size(); in++)
            {
                SpriteComp& sprite = spriteArray.m_Components[in];
                UUID ID = spriteArray.m_IndexToId[in];

                if(transformArray.m_IdToIndex.count(ID))
                {
                    TransformComp& transform = transformArray.m_Components[transformArray.m_IdToIndex[ID]];
                    sprite.SetPosition(transform.GetPosition());
                }

                if(colorArray.m_IdToIndex.count(ID)) 
                {
                    ColorComp& color = colorArray.m_Components[colorArray.m_IdToIndex[ID]];
                    sprite.SetColor(color.m_Color);
                }

                m_Target.draw(sprite.m_Sprite, states);
            }

            for(size_t in = 0; in < textArray.m_Components.size(); in++)
            {
                TextComp& text = textArray.m_Components[in];
                UUID ID = textArray.m_IndexToId[in];

                if(transformArray.m_IdToIndex.count(ID))
                {
                    TransformComp& transform = transformArray.m_Components[transformArray.m_IdToIndex[ID]];
                    text.SetPosition(transform.GetPosition());
                }

                if(colorArray.m_IdToIndex.count(ID)) 
                {
                    ColorComp& color = colorArray.m_Components[colorArray.m_IdToIndex[ID]];
                    text.SetColor(color.m_Color);
                }

                m_Target.draw(text.m_Text, states);
            }
        }

    private:
        sf::RenderWindow& m_Target;
    };
}