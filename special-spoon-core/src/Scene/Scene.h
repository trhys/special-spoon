#pragma once

#include "Core.h"
#include "Node.h"

namespace Spoon 
{
    class Scene : public Node
    {
    public:
        Scene() {}
        Scene(std::string name, sf::Vector2f size) : m_Name(name) { m_Bounds.size = size; }
        virtual ~Scene() {}

        void ShowScene() { m_IsActive = true; }
        void HideScene() { m_IsActive = false; }
        void SetActive(bool set) { m_IsActive = set; }
        bool IsActive() { return m_IsActive; } 

        std::string GetName() { return m_Name; }
        sf::FloatRect GetBounds() const { return m_Bounds; }

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}

        bool m_IsActive = false;
        
        sf::FloatRect m_Bounds;
        std::string m_Name;
    };
}