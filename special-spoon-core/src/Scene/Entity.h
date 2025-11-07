#pragma once

#include "Scene/Node.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public Node
    {
    public:
        Entity() {}
        virtual ~Entity() {}

        virtual void OnAdd() {}
        virtual void OnKill() {}

    private:
        // sf::Texture m_Texture = nullptr; // I don't know yet if Entity is ever going to need it's own texture instance --- it should be owned by resource manager
        sf::Sprite m_Sprite = nullptr;

        void OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const override
        {
            if(m_Sprite)
            {
                target.draw(m_Sprite, transform);
            }
        }
    };
};