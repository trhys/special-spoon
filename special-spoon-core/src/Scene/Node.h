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

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&) = default;
        Node& operator=(Node&&) = default;

        // Recursive updating methods
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void Update(sf::Time tick);

    public:
        // Graph handling \ Child management
        template<typename NODE>
        void AddChild(std::string resource_id)
        {
            sf::Texture& texture = ResourceManager::GetResource<sf::Texture>(resource_id);
            m_Children.emplace_back(std::make_unique<NODE>(texture));
        }

        template<typename NODE>
        void AddChild()
        {
            m_Children.emplace_back(std::make_unique<NODE>());
        }

        template<typename NODE>
        void AddChild(std::string resource_id, sf::Vector2f position)
        {
            sf::Texture& texture = ResourceManager::GetResource<sf::Texture>(resource_id);
            m_Children.emplace_back(std::make_unique<NODE>(texture, position));
        }

        template<typename NODE>
        void AddChild(sf::Vector2f position)
        {
            m_Children.emplace_back(std::make_unique<NODE>(position));
        }

        template<typename NODE>
        void AddText(std::string resource_id, sf::Vector2f position)
        {
            sf::Font& font = ResourceManager::GetResource<sf::Font>(resource_id);
            m_Children.emplace_back(std::make_unique<NODE>(font, position));
        }

        virtual void OnAdd() {}
        void OnKill() { m_IsDead = true; }
        bool IsDead() { return m_IsDead; }
        void Cleanup();
        
        const std::vector<std::unique_ptr<Node>>& GetChildren() { return m_Children; }
        
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