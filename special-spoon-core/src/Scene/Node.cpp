#include "Node.h"
#include "Layer.h"

namespace Spoon
{
    void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        OnDraw(target, states);

        for (auto& child : m_Children)
        {
            child->draw(target, states);
        }
    }

    void Node::Update(sf::Time tick, Layer* context)
    {
        OnUpdate(tick, context);
        for (auto& child : m_Children)
        {
            child->Update(tick, context);
        }
    }
}