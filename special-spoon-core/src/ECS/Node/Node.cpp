#include "Node.h"
#include "ECS/TransComp.h"
#include "ECS/SpriteComp.h"
#include "ECS/PhysComp.h"

namespace Spoon
{
    void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if(HasComponent<TransComp>())
        {
            states.transform *= GetComponent<TransComp>()->GetTransform();
        }

        OnDraw(target, states);

        for (auto& child : m_Children)
        {
            child->draw(target, states);
        }
    }

    void Node::Update(sf::Time tick)
    {
        // Update ECS
        TransComp* transform = GetComponent<TransComp>();
        SpriteComp* sprite = GetComponent<SpriteComp>();
        PhysComp* pbody = GetComponent<PhysComp>();

        if(transform) { transform->m_CurrentPosition = transform->GetPosition(); }

        ComponentUpdate(tick); // Update AFTER saving current position for future collision handling
        
        if(transform && sprite)
        {
            sprite->SetPosition(transform->GetPosition());
        }
        
        if(transform && pbody)
        {
            pbody->SetPosition(transform->GetPosition());
        }

        // Node internal update
        OnUpdate(tick);

        // Recurse through graph
        for (auto& child : m_Children)
        {
            child->Update(tick);
        }
    }

    void Node::ComponentUpdate(sf::Time tick)
    {
        for(auto& comp : m_Components)
        {
            comp->Update(tick);
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
        if(HasComponent<PhysComp>()) { outbuffer.push_back(this); }
        for(auto& child : m_Children)
        {
            child->SendNodes(outbuffer);
        }
    }
}