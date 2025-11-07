#pragma once

#include "SFML/Graphics.hpp"

namespace Spoon 
{    
    class Entity;

    class Node : public sf::Drawable, public sf::Transformable
    {
    public:
        Node() {}
        virtual ~Node() {}

        void CreateEntity();

        void draw(sf::RenderTarget& target, sf::Transform& transform);

    private:
        sf::Transform m_Transform;
        std::vector<Node*> m_Children;

        virtual void OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const = 0;
    };

    class Scene : public Node
    {
    public:
        Scene(sf::Texture texture) : m_Texture(texture), m_Sprite(texture){}
        virtual ~Scene() {}

        void CreateEntity();

    private:
        sf::Texture m_Texture;
        sf::Sprite m_Sprite;

        void OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const override;
    };
}