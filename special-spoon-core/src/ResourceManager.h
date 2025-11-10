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

        void Init(std::unordered_map<std::string, std::filesystem::path> texture_files)
        {
            for (auto const& [key, value] : texture_files)
            {
                m_Textures.emplace(key, sf::Texture::Texture(value));
            }
        }
        void LoadTexture(const std::string texture_name, const std::filesystem::path file_path) { m_Textures.emplace(texture_name, sf::Texture::Texture(file_path)); }

    private:
        std::unordered_map<std::string, sf::Texture> m_Textures;
    };
}