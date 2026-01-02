namespace Spoon
{
    template<typename RESOURCE>
    void ResourceManager::LoadResource(const std::string id, const std::filesystem::path file_path)
    {
        if constexpr (std::is_same_v<RESOURCE, sf::Texture>)
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
                texturePaths[id] = file_path;
            }
        }
        else if constexpr(std::is_same_v<RESOURCE, sf::Font>)
        {
            auto found = m_Fonts.find(id);
            if (found == m_Fonts.end())
            {
                sf::Font font;
                if (!font.openFromFile(file_path))
                {
                    throw std::runtime_error("Failed to load font from file path: " + file_path.string());
                }
                GenerateFontPreview(id, font);
                m_Fonts.emplace(id, std::move(font));
                fontPaths[id] = file_path;
            }
        }
        else if constexpr(std::is_same_v<RESOURCE, sf::SoundBuffer>)
        {
            auto found = m_SoundBuffers.find(id);
            if (found == m_SoundBuffers.end())
            {
                sf::SoundBuffer soundBuffer;
                if (!soundBuffer.loadFromFile(file_path))
                {
                    throw std::runtime_error("Failed to load sound buffer from file path: " + file_path.string());
                }
                m_SoundBuffers.emplace(id, std::move(soundBuffer));
                soundPaths[id] = file_path;
            }
        }
        else
        {
            static_assert(std::is_same_v<RESOURCE, sf::Texture> || std::is_same_v<RESOURCE, sf::Font> || std::is_same_v<RESOURCE, sf::SoundBuffer>, "Unsupported resource type");
        }
    }

    template<typename RESOURCE>
    void ResourceManager::RemoveResource(const std::string id)
    {
        if constexpr (std::is_same_v<RESOURCE, sf::Texture>)
        {
            auto found = m_Textures.find(id);
            if (found != m_Textures.end())
            {
                m_Textures.erase(id);
            }
        }
        else if constexpr(std::is_same_v<RESOURCE, sf::Font>)
        {
            auto found = m_Fonts.find(id);
            if (found != m_Fonts.end())
            {
                
                m_Fonts.erase(id);
            }
        }
        else if constexpr(std::is_same_v<RESOURCE, sf::SoundBuffer>)
        {
            auto found = m_SoundBuffers.find(id);
            if (found != m_SoundBuffers.end())
            {
                m_SoundBuffers.erase(id);
            }
        }
        else
        {
            static_assert(std::is_same_v<RESOURCE, sf::Texture> || std::is_same_v<RESOURCE, sf::Font> || std::is_same_v<RESOURCE, sf::SoundBuffer>, "Unsupported resource type");
        }
    }

    template<typename RESOURCE>
    RESOURCE& ResourceManager::GetResource(std::string id)
    {
        if constexpr (std::is_same_v<RESOURCE, sf::Texture>)
        {
            return m_Textures.at(id);
        }
        else if constexpr(std::is_same_v<RESOURCE, sf::Font>)
        {
            return m_Fonts.at(id);
        }
        else if constexpr(std::is_same_v<RESOURCE, sf::SoundBuffer>)
        {
            return m_SoundBuffers.at(id);
        }
        else
        {
            static_assert(std::is_same_v<RESOURCE, sf::Texture> || std::is_same_v<RESOURCE, sf::Font>, "Unsupported resource type");
        }
    }
}