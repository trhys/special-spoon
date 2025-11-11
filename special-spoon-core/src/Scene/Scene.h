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

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}
        //virtual void OnUpdate(sf::Time tick, Layer* context) override {}
    };
}