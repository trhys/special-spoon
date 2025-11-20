#pragma once

#include "Core.h"

#include "SFML/Graphics.hpp"

#include <vector>

namespace Spoon
{   
    class SPOON_API Node : public sf::Drawable, public sf::Transformable
    {
    public:
        Node() {}
        virtual ~Node() {}

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void Update(sf::Time tick);

        virtual void CollisionDetected() {}      
        virtual sf::FloatRect GetBoundingBox() { return sf::FloatRect(); }
        virtual sf::Texture& LoadTexture(std::string id, std::filesystem::path file_path);
        virtual sf::Font& LoadFont(std::string id, std::filesystem::path file_path);

        void MakeParent(Node* parent);
        void AddChild(Node* child);
        void AddChild(Node* child, sf::Vector2f position);
        void KillChild(Node* child);

        Node* GetParent() { return p_Parent; }
        std::vector<Node*>& GetChildren() { return m_Children; } 

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
        virtual void OnUpdate(sf::Time tick) {}
        
        Node* p_Parent;
        std::vector<Node*> m_Children;
    };
}