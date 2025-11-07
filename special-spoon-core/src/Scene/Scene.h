#pragma once

#include "Scene/Node.h"

#include "ResourceManager.h"

#include "SFML/Graphics.hpp"

namespace Spoon 
{
    class SPOON_API Scene : public Node
    {
    public:
        Scene() {}
        virtual ~Scene() {}

        // Load and unload resources here
        virtual void Load(ResourceManager& rsm);
        virtual void Unload() { m_IsLoaded = false; }
        
        // Set texture here after Load()
        virtual void SetScene() {}; 

    private:
        virtual void OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const override {}

        sf::Texture m_Texture = nullptr;
        sf::Sprite m_Background = nullptr;

        bool m_IsLoaded = false;

        ResourceManager* m_RSM;
        std::map<std::string, std::filesystem::path> m_Resources;
    };
}