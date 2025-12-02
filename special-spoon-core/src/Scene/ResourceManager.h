#pragma once

#include "Core.h"
#include "SFML/Graphics.hpp"
#include <type_traits>

namespace Spoon
{
    class SPOON_API ResourceManager
    {
    public:
        ResourceManager() {}
        ~ResourceManager() {}

        void LoadTexture(const std::string id, const std::filesystem::path file_path);
        void LoadFont(const std::string id, const std::filesystem::path file_path);

        template<typename RESOURCE>
        static RESOURCE& GetResource(std::string id)
        {
            if constexpr (std::is_same_v<RESOURCE, sf::Texture>)
            {
                return m_Textures[id];
            }
            else if constexpr(std::is_same_v<RESOURCE, sf::Font>)
            {
                return m_Fonts[id];
            }
            else
            {
                static_assert(std::is_same_v<RESOURCE, sf::Texture> || std::is_same_v<RESOURCE, sf::Font>, "Unsupported resource type");
            }
        }

    private:
        static std::unordered_map<std::string, sf::Texture> m_Textures;
        static std::unordered_map<std::string, sf::Font> m_Fonts;
    };
}