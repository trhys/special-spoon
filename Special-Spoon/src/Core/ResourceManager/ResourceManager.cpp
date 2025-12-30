#include "ResourceManager.h"
#include "SystemFont/Jersey_10/SystemFont.h"

namespace Spoon
{
    ResourceManager& ResourceManager::Get()
    {
        static ResourceManager manager;
        return manager;
    }

    std::filesystem::path ResourceManager::Normalize(std::filesystem::path p)
    {
        return std::filesystem::absolute(p).lexically_normal();
    }

    std::string ResourceManager::CheckExtension(std::filesystem::path p)
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

    ResourceType ResourceManager::GetType(const std::string& ext)
    {
        static const std::unordered_map<std::string, ResourceType> typeMap = {
            {"Image file", ResourceType::Texture},
            {"Font file",  ResourceType::Font},
            {"Sound file", ResourceType::Sound}                
        };
    
        auto it = typeMap.find(ext);
        return (it != typeMap.end()) ? it->second : ResourceType::Unknown;
    }

    void ResourceManager::ScanAssets(const std::filesystem::path& rootDir)
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

    void ResourceManager::LoadAnimationData(const std::string& id, const AnimationData& animationData)
    {
        auto found = m_Animations.find(id);
        if(found == m_Animations.end())
        {
            m_Animations.emplace(id, animationData);
        }
    }

    AnimationData* ResourceManager::GetAnimationData(const std::string& id)
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

    void ResourceManager::InitDefaultAssets()
    {
        m_Textures["empty"] = sf::Texture();
        sf::Font& font = m_Fonts["Default"];
        if (!font.openFromMemory(Jersey10_Regular_ttf, Jersey10_Regular_ttf_len))
        {
            throw std::runtime_error("Failed to load system font!");
        }
        GenerateFontPreview("Default", font);
    }

    void ResourceManager::GenerateFontPreview(const std::string& id, sf::Font& font)
    {
        sf::RenderTexture fontViewer({200, 40});
        fontViewer.clear(sf::Color::Transparent);
        sf::Text text(font, id, 24);
        fontViewer.draw(text);
        fontViewer.display();
        m_FontPreviews[id] = fontViewer.getTexture();
    }

    sf::Texture& ResourceManager::GetFontPreview(const std::string& id)
    {
        auto found = m_FontPreviews.find(id);
        if (found != m_FontPreviews.end())
            return found->second;
        else
            return m_FontPreviews.at("Default");
    }

    void ResourceManager::ClearAllResources()
    {
        m_Textures.clear();
        m_Fonts.clear();
        m_Animations.clear();
        m_SoundBuffers.clear();

        InitDefaultAssets();
    }
}