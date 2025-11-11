#pragma once

#include "Core.h"
#include "Node.h"

namespace Spoon 
{
    class Scene : public Node
    {
    public:
        Scene() {}
        virtual ~Scene() {}

        void SetBounds(sf::Vector2f position, sf::Vector2f size)
        {
            m_Bounds.position = position;
            m_Bounds.size = size;
        }

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}

        sf::FloatRect m_Bounds;
    };
}