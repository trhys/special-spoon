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
        child->OnAdd();
    }

    void Node::AddChild(Node* child, sf::Vector2f position)
    {
        child->MakeParent(this);
        child->setPosition(position);
        m_Children.push_back(child);
        child->OnAdd();
    }

    void Node::RemoveDead()
    {
        auto remove_it = std::remove_if(m_Children.begin(), m_Children.end(), [](Node* child){ return child->IsDead(); });
        for(auto it = remove_it; it != m_Children.end(); it++)
        {
            delete *it;
        }
        m_Children.erase(remove_it, m_Children.end());
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

    sf::Texture& Node::LoadTexture(std::string id, std::filesystem::path file_path)
    {
        sf::Texture texture;
        return texture;
    }

    sf::Font& Node::LoadFont(std::string id, std::filesystem::path file_path)
    {
        sf::Font font;
        return font;
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