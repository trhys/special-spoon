#pragma once

#include <vector>
#include <algorithm>
#include "SFML/Graphics.hpp"


namespace Spoon
{   
    class Layer;

    class Node : public sf::Drawable, public sf::Transformable
    {
    public:
        Node() {}
        virtual ~Node() {}

        template <typename T>
        void AddChildNode(T child)
        {
            m_Children.emplace_back(child);
        }

        template <typename T>
        void KillChildNode(T* child)
        {
            erase(m_Children, child);
            delete child;
        }

        std::vector<Node*> GetChildren() { return m_Children; }

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            states.transform *= getTransform();

            OnDraw(target, states);

            for (auto& child : m_Children)
            {
                child->draw(target, states);
            }
        }

        void Update(sf::Time tick, Layer* context)
        {
            OnUpdate(tick, context);
            for (auto& child : m_Children)
            {
                child->Update(tick, context);
            }
        }

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
        virtual void OnUpdate(sf::Time tick, Layer* context) {}

        std::vector<Node*> m_Children;
    };
}