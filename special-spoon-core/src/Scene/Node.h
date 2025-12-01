#pragma once

#include "Core.h"
#include "ResourceManager.h"
#include "SFML/Graphics.hpp"
#include <vector>
#include <optional>
#include <memory>

namespace Spoon
{   
    class SPOON_API Node : public sf::Drawable, public sf::Transformable
    {
    public:
        Node() {}
        Node(bool collidable) : m_IsCollidable(collidable) {}
        virtual ~Node() {}

        // Recursive updating methods
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void Update(sf::Time tick);

    public:
        // Graph handling \ Child management
        template<typename NODE, typename RESOURCE>
        void AddChild(std::optional<std::string> resource_id = std::nullopt,
                      std::optional<sf::Vector2f> position = std::nullopt)
        {
            if(resource_id.has_value() && position.has_value())
            {
                RESOURCE& asset = ResourceManager::GetResource<RESOURCE>(resource_id.value());
                m_Children.emplace_back(std::make_unique<NODE>(asset, position.value()));
            }
            else if(resource_id.has_value())
            {
                RESOURCE& asset = ResourceManager::GetResource<RESOURCE>(resource_id.value());
                m_Children.emplace_back(std::make_unique<NODE>(asset));
            }
            else if(position.has_value())
            {
                m_Children.emplace_back(std::make_unique<NODE>(position.value()));
            }
            else
            {
                m_Children.emplace_back(std::make_unique<NODE>());
            }
        }

        virtual void OnAdd() {}
        void OnKill() { m_IsDead = true; }
        bool IsDead() { return m_IsDead; }
        void Cleanup();
        
        std::vector<Node*>& GetChildren() { return m_Children; }
        
    public:
        // Physics
        virtual sf::FloatRect GetBoundingBox() { return sf::FloatRect(); }
        virtual void CollisionDetected() {}
        bool GetIsCollidable() { return m_IsCollidable; }
        void SendNodes(std::vector<Node*>& outbuffer);
         
    private:
        // Recursive update methods
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
        virtual void OnUpdate(sf::Time tick) {}

        // Remove dead child nodes at end of update phase
        void RemoveDead();

    private:
        bool m_IsDead = false;
        bool m_IsCollidable = false;

        std::vector<std::unique_ptr<Node>> m_Children;
    };
}