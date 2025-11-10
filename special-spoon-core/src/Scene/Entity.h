#pragma once

#include "Node.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public Node
    {
    public:
        Entity() {}
        virtual ~Entity() {}

        sf::Sprite GetSprite() { return *m_Sprite; }
        virtual void OnAdd() {}
        virtual void OnKill() {}

    private:
        sf::Sprite* m_Sprite = nullptr;

        void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(*m_Sprite, states);
        }
    };
};