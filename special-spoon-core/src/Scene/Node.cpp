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

    void Node::KillChild(Node* child)
    {
        m_Children.erase(remove(m_Children.begin(), m_Children.end(), child), m_Children.end());
        child->OnKill();
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
}