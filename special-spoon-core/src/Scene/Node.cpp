#include "Node.h"

namespace Spoon
{
    void Node::MakeParent(Node* parent)
    {
        p_Parent = parent;
    }

    void Node::AddChild(Node* child)
    {
        child->MakeParent(this);
        m_Children.push_back(child);
    }

    void Node::AddChild(Node* child, sf::Vector2f position)
    {
        child->MakeParent(this);
        child->setPosition(position);
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