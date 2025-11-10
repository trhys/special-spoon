#pragma once

#include "Core.h"
#include "Node.h"

#include "ResourceManager.h"

#include "SFML/Graphics.hpp"
#include <map>

namespace Spoon 
{
    class SPOON_API Scene : public Node
    {
    public:
        Scene() {}
        virtual ~Scene() {}

        // Load and unload resources here
        virtual void Load(ResourceManager* rsm);
        virtual void Unload() { m_IsLoaded = false; }
        
        // Set texture here after Load()
        void SetScene(sf::Texture texture) {  }

    private:
        virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(*m_Background, states); }

        sf::Sprite* m_Background = nullptr;

        bool m_IsLoaded = false;

        ResourceManager* m_RSM = nullptr;
        std::map<std::string, std::filesystem::path> m_Resources;
    };
}