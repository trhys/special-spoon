#include "Node.h"
#include "Layer.h"


namespace Spoon
{
    void Node::AddChild(Node* child)
    {
        m_Children.push_back(child);
    }

    void Node::KillChild(Node* child)
    {
        m_Children.erase(remove(m_Children.begin(), m_Children.end(), child), m_Children.end());
        delete child;
    }

    void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        OnDraw(target, states);

        for (auto& child : m_Children)
        {
            child->draw(target, states);
        }
    }

    void Node::Update(sf::Time tick)
    {
        OnUpdate(tick);
        for (auto& child : m_Children)
        {
            child->Update(tick);
        }
    }
}