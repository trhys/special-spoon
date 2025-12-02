#include "ResourceManager.h"

namespace Spoon
{
    void ResourceManager::LoadTexture(const std::string id, const std::filesystem::path file_path)
    {
        auto found = m_Textures.find(id);
        if (found == m_Textures.end())
        {
            sf::Texture texture;
            if (!texture.loadFromFile(file_path))
            {
                throw std::runtime_error("Failed to load texture from file path: " + file_path.string());
            }
            m_Textures.emplace(id, std::move(texture));
        }
    }

    void ResourceManager::LoadFont(const std::string id, const std::filesystem::path file_path)
    {
        auto found = m_Fonts.find(id);
        if (found == m_Fonts.end())
        {
            sf::Font font;
            if (!font.openFromFile(file_path))
            {
                throw std::runtime_error("Failed to load font from file path: " + file_path.string());
            }
            m_Fonts.emplace(id, std::move(font));
        }
    }

    std::unordered_map<std::string, sf::Texture> ResourceManager::m_Textures;
    std::unordered_map<std::string, sf::Font> ResourceManager::m_Fonts;
}