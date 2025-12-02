#include "Node.h"

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

    void Node::Update(sf::Time tick)
    {
        OnUpdate(tick);
        for (auto& child : m_Children)
        {
            child->Update(tick);
        }
    }

    void Node::RemoveDead()
    {
        auto remove_it = std::remove_if(m_Children.begin(), m_Children.end(), [](const auto& child){ return child->IsDead(); });
        m_Children.erase(remove_it, m_Children.end());
    }

    void Node::Cleanup()
    {
        for(auto& child : m_Children)
        {
            child->Cleanup();
        }
        RemoveDead();
    }

    void Node::SendNodes(std::vector<Node*>& outbuffer)
    {
        if(m_IsCollidable) { outbuffer.push_back(this); }
        for(auto& child : m_Children)
        {
            child->SendNodes(outbuffer);
        }
    }
}