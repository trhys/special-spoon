#pragma once

#include "Core.h"
#include "ECS/Components/Animation/AnimationData.h"

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <type_traits>
#include <filesystem>
#include <memory>
#include <map>

namespace Spoon
{
    // For populating the editor's resource view
    // ScanAssets() recurses the working directory and
    // sets the fileRoot member to the top of this node graph 
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
        ResourceManager() {}
        ~ResourceManager() {}

        static std::filesystem::path Normalize(std::filesystem::path p)
        {
            return std::filesystem::absolute(p).lexically_normal();
        }

        static std::string CheckExtension(std::filesystem::path p)
        {
            if(std::filesystem::is_directory(p))
                return "Folder";

            std::string extension = p.extension().string();
            for(auto& c : extension)
            {
                c = std::tolower(c);
            }
            if(extension == ".png" || extension == ".jpeg" || extension == ".jpg" || extension == ".svg")
                return "Image file";

            if(extension == ".wav" || extension == ".mp3")
                return "Sound file";

            if(extension == ".txt")
                return "Text file";

            if(extension == ".ttf")
                return "Font file";
            
            return "Unknown";
        }

        static void ScanAssets(const std::filesystem::path& rootDir)
        {
            auto normalRoot = Normalize(rootDir);
            fileRoot = std::make_unique<AssetNode>("Assets", normalRoot, "Root", true);
            
            std::map<std::filesystem::path, AssetNode*> dirMap;
            dirMap[""] = fileRoot.get();

            for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDir))
            {
                auto entryP = std::filesystem::relative(entry.path(), normalRoot);
                auto parentP = entryP.parent_path();

                auto it = dirMap.find(parentP);
                if (it == dirMap.end()) {
                    continue;
                }

                AssetNode* parentNode = it->second;

                std::unique_ptr<AssetNode> newNode = std::make_unique<AssetNode>(
                    entryP.stem().string(), 
                    entry.path(),
                    CheckExtension(entryP),
                    entry.is_directory()
                );

                AssetNode* newNodePtr = newNode.get();

                parentNode->m_Children.push_back(std::move(newNode));
                
                if (entry.is_directory())
                {
                    dirMap[entryP] = newNodePtr;
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

        static const std::unordered_map<std::string, sf::Texture>& GetTextures()
        {
            return m_Textures;
        }
        
        static void ClearAllResources()
        {
            m_Textures.clear();
            m_Fonts.clear();
            m_Animations.clear();
            m_SoundBuffers.clear();
        }
        
        static AssetNode* GetAssetsDir() { return fileRoot.get(); }

    private:
        static inline std::unordered_map<std::string, sf::Texture> m_Textures;
        static inline std::unordered_map<std::string, sf::Font> m_Fonts;
        static inline std::unordered_map<std::string, AnimationData> m_Animations;
        static inline std::unordered_map<std::string, sf::SoundBuffer> m_SoundBuffers;

        static inline std::unique_ptr<AssetNode> fileRoot;
    };
}