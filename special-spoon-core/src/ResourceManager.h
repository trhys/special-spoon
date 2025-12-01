#pragma once

#include "Core.h"
#include "SFML/Graphics.hpp"
#include <type_traits>

namespace Spoon
{
    class ResourceManager
    {
    public:
        ResourceManager() {}
        ~ResourceManager() {}

        void LoadTexture(const std::string id, const std::filesystem::path file_path)
        {
            auto found = m_Textures.find(id);
            if(found == m_Textures.end())
            {
                sf::Texture texture;
                if(!texture.loadFromFile(file_path))
                {
                    throw std::runtime_error("Failed to load texture from file path: " + file_path.string());
                }
                m_Textures.emplace(id, std::move(texture));
            }
        }

        void LoadFont(const std::string id, const std::filesystem::path file_path)
        {
            auto found = m_Fonts.find(id);
            if(found == m_Fonts.end())
            {
                sf::Font font;
                if(!font.openFromFile(file_path))
                {
                    throw std::runtime_error("Failed to load font from file path: " + file_path.string());
                }
                m_Fonts.emplace(id, std::move(font));
            }
        }

        template<typename RESOURCE>
        static RESOURCE& GetResource(std::string id)
        {
            if constexpr(std::is_same_v(RESOURCE, sf::Texture))
            {
                return m_Textures[id];
            }
            else if constexpr(std::is_same_v(RESOURCE, sf::Font))
            {
                return m_Fonts[id];
            }
        }

    private:
        static std::unordered_map<std::string, sf::Texture> m_Textures;
        static std::unordered_map<std::string, sf::Font> m_Fonts;
    };
}