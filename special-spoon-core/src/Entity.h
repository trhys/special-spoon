#pragma once

#include "Scene.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public Spoon::Node
    {
    public:
        Entity(sf::Texture texture) : m_Texture(texture), m_Sprite(m_Texture) {}
        virtual ~Entity() {}

        virtual void OnAdd() {}
        virtual void OnKill() {}

        //sf::Sprite& GetSprite() { return m_Sprite; }

    private:
        sf::Texture m_Texture;
        sf::Sprite m_Sprite;

        void OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const override
        {
            target.draw(m_Sprite, transform);
        }
    };
};