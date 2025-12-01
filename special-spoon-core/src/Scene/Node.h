#pragma once

#include "Core.h"
#include "ResourceManager.h"
#include "SFML/Graphics.hpp"
#include <vector>
#include <optional>

namespace Spoon
{   
    class SPOON_API Node : public sf::Drawable, public sf::Transformable
    {
    public:
        Node() {}
        Node(bool collidable) : m_IsCollidable(collidable) {}
        virtual ~Node() {}

        // Scene updating
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void Update(sf::Time tick);

        // Graph handling
        template<typename NODE, typename RESOURCE>
        void AddChild(std::optional<std::string> resource_id = std::nullopt,
                      std::optional<sf::Vector2f> position = std::nullopt)
        {
            if(resource_id.has_value())
            {
                RESOURCE& asset = ResourceManager::GetResource<RESOURCE>(resource_id);
                m_Children.emplace_back(new NODE(asset));
            }
            else if(resource_id.has_value() && position.has_value())
            {
                RESOURCE& asset = ResourceManager::GetResource<RESOURCE>(resource_id);
                m_Children.emplace_back(new NODE(asset, position));
            }
            else if(position.has_value())
            {
                m_Children.emplace_back(new NODE(position));
            }
            else
            {
                m_Children.emplace_back(new NODE());
            }
        }

        virtual void OnAdd() {}
        void OnKill() { m_IsDead = true; }
        bool IsDead() { return m_IsDead; }
        void Cleanup();
        
        std::vector<Node*>& GetChildren() { return m_Children; }
        
        // Physics
        virtual sf::FloatRect GetBoundingBox() { return sf::FloatRect(); }
        virtual void CollisionDetected() {}
        bool GetIsCollidable() { return m_IsCollidable; }
        void SendNodes(std::vector<Node*>& outbuffer);
         
    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
        virtual void OnUpdate(sf::Time tick) {}
        void RemoveDead();

        bool m_IsDead = false;
        bool m_IsCollidable = false;

        Node* p_Parent;
        std::vector<Node*> m_Children;
    };
}