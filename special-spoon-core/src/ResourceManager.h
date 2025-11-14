#pragma once

#include "Core.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    class ResourceManager
    {
    public:
        ResourceManager() {}
        ~ResourceManager() {}

        void Init(Application* app) { p_App = app; }

        sf::Texture& LoadTexture(const std::string id, const std::filesystem::path file_path)
        {
            auto found = m_Textures.find(id);
            if(found == m_Textures.end())
            {
                sf::Texture texture;
                if(!texture->loadFromFile(file_path))
                {
                    throw std::runtime_error("Failed to load texture from file path: " + file_path.string());
                }
                m_Textures.emplace(id, std::move(texture));
                return m_Textures[id];
            }
        }

        sf::Texture& GetTexture(const std::string id)
        {
            return m_Textures[id];
        }

    private:
        Application* p_App = nullptr;
        std::unordered_map<std::string, sf::Texture> m_Textures;
    };
}