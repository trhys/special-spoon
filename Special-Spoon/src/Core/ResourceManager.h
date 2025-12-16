#pragma once

#include "Core.h"
#include "ECS/Components/Animation/AnimationData.h"

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <type_traits>
#include <filesystem>

namespace Spoon
{
    struct AssetNode
    {
        AssetNode(std::string name, std::filesystem::path path, bool dir)
            : m_Name(name), m_Path(path), isDir(dir) {}
        std::string m_Name;
        std::filesystem::path m_Path;
        bool isDir;
        std::vector<std::unique_ptr<AssetNode>> m_Children;
    }

    class SPOON_API ResourceManager
    {
    public:
        ResourceManager() {}
        ~ResourceManager() {}

        static void ScanAssets(const std::filesystem::path& rootDir)
        {
            fileRoot = std::make_unique<AssetNode>(rootDir.stem().string(), rootDir.path(), true);
            
            std::map<fs::path, AssetNode*> dirMap;
            dirMap[rootDir] = fileRoot.get();

            for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDir))
            {
                std::filesystem::path entryP = entry.path();
                std::filesystem::path parentP = entryP.path();

                AssetNode* parentNode = dirMap[parentP];

                std::unique_ptr<AssetNode> newNode = std::make_unique<AssetNode>(
                    entry.stem().string(), 
                    entryP, 
                    entry.is_directory()
                );

                AssetNode* newNodePtr = newNode.get();

                parentNode->m_Children.push_back(std::move(newNode));

                if (entry.is_directory())
                {
                    dirMap[entryPath] = newNodePtr;
                }
            }
        }

        template<typename RESOURCE>
        static void LoadResource(const std::string id, const std::filesystem::path file_path)
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
                    m_TexturePaths[id] = file_path;
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
                    m_Fonts.emplace(id, std::move(font));
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
                }
            }
            else
            {
                static_assert(std::is_same_v<RESOURCE, sf::Texture> || std::is_same_v<RESOURCE, sf::Font>, "Unsupported resource type");
            }
        }

        template<typename RESOURCE>
        static RESOURCE& GetResource(std::string id)
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

        static void LoadAnimationData(const std::string id, const AnimationData& animationData)
        {
            auto found = m_Animations.find(id);
            if(found == m_Animations.end())
            {
                m_Animations.emplace(id, animationData);
            }
        }

        static AnimationData* GetAnimationData(const std::string id)
        {
            auto found = m_Animations.find(id);
            if(found != m_Animations.end())
            {
                return &found->second;
            }
            else
            {
                throw std::runtime_error("AnimationID: " + id + " not found.");
            }
        }
        
        static void ClearAllResources()
        {
            m_Textures.clear();
            m_Fonts.clear();
            m_Animations.clear();
            m_SoundBuffers.clear();
        }
        
        static const std::unordered_map<std::string, std::filesystem::path>& GetTextureMap() { return m_TexturePaths; }

    private:
        static inline std::unordered_map<std::string, sf::Texture> m_Textures;
        static inline std::unordered_map<std::string, std::filesystem::path> m_TexturePaths;
        static inline std::unordered_map<std::string, sf::Font> m_Fonts;
        static inline std::unordered_map<std::string, AnimationData> m_Animations;
        static inline std::unordered_map<std::string, sf::SoundBuffer> m_SoundBuffers;

        static inline std::unique_ptr<AssetNode> fileRoot;
    };
}