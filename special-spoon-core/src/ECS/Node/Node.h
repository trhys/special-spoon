#pragma once

#include "Core.h"
#include "ECS/Component.h"

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
        virtual ~Node();

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&) noexcept;
        Node& operator=(Node&&) noexcept;

        // Recursive updating methods
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void Update(sf::Time tick);
        void ComponentUpdate(sf::Time tick);

    public:
        // Scene Graph System
        template<typename NODE, typename... Args>
        void AddChild(Args&&... args) { m_Children.emplace_back(std::make_unique<NODE>(std::forward<Args>(args)...)); }

        virtual void OnAdd() {}
        virtual void OnDeath() {}
        void OnKill() { m_IsDead = true; }
        bool IsDead() { return m_IsDead; }
        void Cleanup();
        
        const std::vector<std::unique_ptr<Node>>& GetChildren() { return m_Children; }
        
    public:
        // Entity Component System
        template<typename COMP, typename... Args>
        void AddComponent(Args&&... args) { m_Components.emplace_back(std::make_unique<COMP>(std::forward<Args>(args)...)); }

        template<typename COMP>
        COMP* GetComponent() const
        {
            for(const auto& comp : m_Components)
            {
                COMP* found = dynamic_cast<COMP*>(comp.get());
                if(found != nullptr) { return found; }
            }
            return nullptr;
        }

        template<typename COMP>
        bool HasComponent() const { return GetComponent<COMP>() != nullptr; }

    public:
        // Physics helpers
        virtual void CollisionDetected() {}
        void SendNodes(std::vector<Node*>& outbuffer);
         
    private:
        // Recursive update methods
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
        virtual void OnUpdate(sf::Time tick) {}

        // Remove dead child nodes at end of update phase
        void RemoveDead();

    private:
        bool m_IsDead = false;

        std::vector<std::unique_ptr<Node>> m_Children;
        std::vector<std::unique_ptr<Component>> m_Components;
    };
}