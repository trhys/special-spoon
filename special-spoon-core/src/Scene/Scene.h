#pragma once

#include "Core.h"
#include "Node.h"

namespace Spoon 
{
    class ResourceManager;

    class Scene : public Node
    {
    public:
        Scene() {}
        Scene(std::string name, sf::Vector2f size) : m_Name(name) { m_Bounds.size = size; }
        virtual ~Scene() {}

        void ShowScene();
        void HideScene();
        bool IsActive();

        std::string GetName() const { return m_Name; }
        sf::FloatRect GetBounds() const { return m_Bounds; }

        void GetRM(ResourceManager* rm);
        sf::Texture& LoadTexture(std::string id, std::filesystem::path file_path) override;

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override {}

        bool m_IsActive = false;
        ResourceManager* p_RM = nullptr;

        sf::FloatRect m_Bounds;
        std::string m_Name;
    };
}