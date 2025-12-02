#pragma once

#include "Node.h"
#include "ECS/TextComp.h"
#include "ECS/SpriteComp.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public Node
    {
    public:
        Entity() {}
        virtual ~Entity() {}

        virtual void OnAdd() {}
        virtual void OnDeath() {}

    private:
        void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            if(HasComponent<SpriteComp>())
            {
                target.draw(GetComponent<SpriteComp>()->GetSprite(), states);
            }
            if(HasComponent<TextComp>())
            {
                target.draw(GetComponent<TextComp>()->GetText(), states);
            }
        }

        virtual void OnUpdate(sf::Time tick) {}
    };
}