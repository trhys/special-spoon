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

        void LoadTexture(const std::string texture_name, const std::filesystem::path file_path) { m_Textures.emplace(texture_name, sf::Texture texture(file_path)); }

    private:
        std::unordered_map<std::string, sf::Texture*> m_Textures;
    };
}