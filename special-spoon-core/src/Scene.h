#pragma once

#include "SFML/Graphics.hpp"

namespace Spoon 
{    
    class Entity;

    class Scene : public sf::Drawable, public sf::Transformable
    {
    public:
        Scene() {}
        virtual ~Scene() {}

        void CreateEntity();

        void draw(sf::RenderTarget& target, sf::Transform& scene_transform) const;

    private:
        sf::Transform m_Transform;
        std::vector<Scene*> m_Children;

        virtual void OnDraw(sf::RenderTarget& target, const sf::Transform transform) = 0;
    };
}