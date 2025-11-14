#pragma once

#include "Core.h"
#include <vector>
#include <algorithm>
#include "SFML/Graphics.hpp"


namespace Spoon
{   
    class Layer;

    class SPOON_API Node : public sf::Drawable, public sf::Transformable
    {
    public:
        Node() {}
        virtual ~Node() {}

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void Update(sf::Time tick, Layer* context);

        std::vector<Node*> GetChildren() const { return m_Children; }       
        sf::FloatRect GetBoundingBox() { return sf::FloatRect(); }

        virtual void CollisionDetected() {}

        void AddChild(Node* child);
        void KillChild(Node* child);

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
        virtual void OnUpdate(sf::Time tick, Layer* context) {}

        std::vector<Node*> m_Children;
    };
}