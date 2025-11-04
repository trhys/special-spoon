#pragma once

#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public sf::Drawable
    {
    public:
        Entity() {}
        virtual ~Entity() {}

        virtual void OnAdd() {}
        virtual void OnKill() {}

        void draw(sf::RenderTarget& target) const override
        {
            target.draw(m_Sprite);
        }

    private:
        sf::Texture m_Texture;
        sf::Sprite m_Sprite;
    };
};