#pragma once

#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public sf::Drawable
    {
    public:
        Entity(sf::Texture texture) : m_Texture(texture), m_Sprite(m_Texture) {}
        virtual ~Entity() {}

        virtual void OnAdd() {}
        virtual void OnKill() {}

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(m_Sprite, states);
        }

        sf::Sprite& GetSprite() { return m_Sprite; }

    private:
        sf::Texture m_Texture;
        sf::Sprite m_Sprite;
    };
};