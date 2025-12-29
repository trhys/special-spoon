#pragma once

#include "Core/Core.h"
#include "ECS/Components/Animation/AnimationData.h"

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <type_traits>
#include <filesystem>
#include <memory>
#include <map>

namespace Spoon
{
    enum class ResourceType { Unknown, Texture, Font, Sound  };
    struct AssetNode
    {
        AssetNode(std::string name, std::filesystem::path path, std::string extension = "", bool dir = true)
            : m_Name(name), m_Path(path), m_Ext(extension), isDir(dir) 
        {
            if(isDir) { return; }
            try {
                auto size = std::filesystem::file_size(std::filesystem::absolute(path));
                if(size >= 1024 * 1024)
                {
                    int sizeInMB = size / (1024 * 1024);
                    m_Size = std::to_string(sizeInMB) + "MB";
                }
                else 
                {
                    auto sizeInKB = size / 1024;
                    m_Size = std::to_string(sizeInKB) + "KB";
                }
            } catch (const std::filesystem::filesystem_error& e) { m_Size = "Error"; }
        }

        std::string m_Name;
        std::filesystem::path m_Path;
        std::string m_Ext;
        bool isDir;
        std::string m_Size;
        std::vector<std::unique_ptr<AssetNode>> m_Children;
    };

    class SPOON_API ResourceManager
    {
    public:
        ResourceManager() 
        {
            m_Textures["empty"] = sf::Texture();
            m_Fonts["empty"] = sf::Font();
        }
        ~ResourceManager() { ClearAllResources(); }

        static ResourceManager& Get();

        std::filesystem::path Normalize(std::filesystem::path p);
        std::string CheckExtension(std::filesystem::path p);
        ResourceType GetType(const std::string& ext);
        void ScanAssets(const std::filesystem::path& rootDir);

        template<typename RESOURCE>
        void LoadResource(const std::string id, const std::filesystem::path file_path);

        template<typename RESOURCE>
        void RemoveResource(const std::string id);

        template<typename RESOURCE>
        RESOURCE& GetResource(std::string id);

        void LoadAnimationData(const std::string id, const AnimationData& animationData);
        AnimationData* GetAnimationData(const std::string id);

        const std::unordered_map<std::string, sf::Texture>& GetTextures()
        {
            return m_Textures;
        }

        const std::unordered_map<std::string, std::filesystem::path>& GetTexturePaths()
        {
            return texturePaths;
        }

        const std::unordered_map<std::string, sf::Font>& GetFonts()
        {
            return m_Fonts;
        }

        const std::unordered_map<std::string, std::filesystem::path>& GetFontPaths()
        {
            return fontPaths;
        }

        const std::unordered_map<std::string, sf::SoundBuffer>& GetSounds()
        {
            return m_SoundBuffers;
        }

        const std::unordered_map<std::string, std::filesystem::path>& GetSoundPaths()
        {
            return soundPaths;
        }

        const std::unordered_map<std::string, AnimationData>& GetAnimations()
        {
            return m_Animations;
        }

        void ClearAllResources()
        {
            m_Textures.clear();
            m_Fonts.clear();
            m_Animations.clear();
            m_SoundBuffers.clear();
        }
        
        AssetNode* GetAssetsDir() { return fileRoot.get(); }

    private:
        std::unordered_map<std::string, sf::Texture> m_Textures;
        std::unordered_map<std::string, std::filesystem::path> texturePaths;

        std::unordered_map<std::string, sf::Font> m_Fonts;
        std::unordered_map<std::string, std::filesystem::path> fontPaths;

        std::unordered_map<std::string, sf::SoundBuffer> m_SoundBuffers;
        std::unordered_map<std::string, std::filesystem::path> soundPaths;

        std::unordered_map<std::string, AnimationData> m_Animations;

        std::unique_ptr<AssetNode> fileRoot;
    };
}

#include "ResourceManager.inl"