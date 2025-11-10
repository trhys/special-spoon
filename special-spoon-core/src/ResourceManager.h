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

        sf::Texture& AqcuireTexture(const std::string id, const std::filesystem::path file_path)
        {
            auto found = m_Textures.find(id);
            if(found = m_Textures.end())
            {
                m_Textures.emplace(id, file_path);
            }
            return m_Textures[id];
        }

    private:
        std::unordered_map<std::string, sf::Texture> m_Textures;
    };
}