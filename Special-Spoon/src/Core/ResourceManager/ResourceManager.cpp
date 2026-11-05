#include "ResourceManager.h"
#include "SystemFont/Jersey_10/SystemFont.h"
#include "special_spoon_png.h"
#include "folder_png.h"

using json = nlohmann::json;

namespace Spoon
{
    ResourceManager& ResourceManager::Get()
    {
        static ResourceManager* manager = new ResourceManager();
        return *manager;
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
        if (std::filesystem::create_directories(rootDir))
        {
            SS_DEBUG_LOG("[RESOURCE MANAGER] Created assets directory: " + rootDir.string())
        }
        auto normalRoot = Normalize(rootDir);
        auto newRoot = std::make_unique<AssetNode>("Assets", normalRoot, "Root", true);
        
        std::map<std::filesystem::path, AssetNode*> dirMap;
        dirMap[""] = newRoot.get();

        for (const auto& entry : std::filesystem::recursive_directory_iterator(normalRoot))
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

            newNode->m_Parent = parentNode;
            // generate font preview if it's a font file
            if (newNode->m_Ext == "Font file")
                GenerateFontPreview(newNode.get());

            // move into parent
            parentNode->m_Children.push_back(std::move(newNode));
            
            if (entry.is_directory())
            {
                AssetNode* newNodePtr = parentNode->m_Children.back().get();
                dirMap[entryP] = newNodePtr;
            }
        }
        fileRoot = std::move(newRoot);
    }

    void ResourceManager::RefreshAssets(const std::filesystem::path& rootDir)
    {
        // save thumbnails temporarily in case scanassets throws
        auto savedFontPreviews = std::move(m_FontPreviews);
        auto savedThumbnails = std::move(m_Thumbnails);
        m_FontPreviews.clear();
        m_Thumbnails.clear();
        try {
            ScanAssets(rootDir);
        } catch (...)
        {
            m_FontPreviews = std::move(savedFontPreviews);
            m_Thumbnails = std::move(savedThumbnails);
            throw;
        }
        InitDefaultFontPreview();
    }

    void ResourceManager::ResetAssetsDirectory(const std::filesystem::path& newRoot)
    {
        fileRoot = std::make_unique<AssetNode>(
            "assets", newRoot, "Root", true);

        m_Thumbnails.clear();
        m_FontPreviews.clear();
        InitDefaultFontPreview();
    }

    // loads animation into scene from animation data
    bool ResourceManager::LoadAnimationData(const std::string& id, const AnimationData& animationData)
    {
        // make sure the animation's texture resource is loaded
        if (!m_Textures.count(animationData.textureID))
        {
            SS_DEBUG_LOG("[RESOURCE MANAGER] Texture for animation not loaded: " + animationData.textureID)
            SS_DEBUG_LOG("[RESOURCE MANAGER] Attempting to load texture ->")
            AssetNode* textureNode = nullptr;
            for (auto* node : GetAssetLibrary("Image file"))
            {
                if (node->m_Name != animationData.textureID)
                    continue;
                if (textureNode)
                    SS_DEBUG_LOG("[RESOURCE MANAGER] Multiple textures found for animation: " + animationData.textureID + " -- Using final match.")
                textureNode = node;
                break;
            }

            if (!textureNode)
            {
                SS_DEBUG_LOG("[RESOURCE MANAGER] Failed to find texture for animation: " + animationData.textureID)
                return false;
            }

            // texture file found, attempt to load it
            LoadResource<sf::Texture>(animationData.textureID, textureNode->m_Path);
            if (!m_Textures.count(animationData.textureID))
                return false;
        }

        // always refresh the cached entry - animations can be re-exported/re-imported
        // with updated spriteCords/frameRate/etc. after a scene has already loaded and
        // cached the old data, so an id match here must overwrite rather than no-op.
        auto found = m_Animations.find(id);
        if (found == m_Animations.end())
        {
            m_Animations.emplace(id, animationData);
        }
        else
        {
            found->second = animationData;
        }
        return true;
    }

    // loads animation from file - file is a json containing animation data
    bool ResourceManager::LoadAnimationFromPath(const std::filesystem::path& file_path)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            SS_DEBUG_LOG("[RESOURCE MANAGER] Failed to open animation file: " + file_path.string())
            return false;
        }
        try 
        {
            json j;
            file >> j;
            file.close();
            AnimationData animationData;
            from_json(j, animationData);
            std::string id = file_path.stem().string();
            animationData.ID = id;
            if (!LoadAnimationData(id, animationData))
                return false;
        }
        catch (const std::exception& e)
        {
            SS_DEBUG_LOG("[RESOURCE MANAGER] Failed to load animation from file: " + file_path.string() + " Error: " + e.what());
            return false;
        }
        return true;
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
        sf::Texture texture;
        if (!texture.loadFromMemory(
            SystemAssets::special_spoon_png, 
            sizeof(SystemAssets::special_spoon_png)))
        {
            throw std::runtime_error("Failed to load default texture!");
        }
        m_Textures.emplace("special-spoon-logo", std::move(texture));

        // load folder icon for editor
        std::filesystem::path folderIconPath = std::filesystem::path("assets")  / "system" / "folder.png";
        if (!texture.loadFromMemory(
            SystemAssets::folder_png, 
            sizeof(SystemAssets::folder_png)))
        {
            throw std::runtime_error("Failed to load folder icon!");
        }
        m_Textures.emplace("special-spoon-folder-icon", std::move(texture));

        sf::Font& font = m_Fonts["Default"];
        if (!font.openFromMemory(Jersey10_Regular_ttf, Jersey10_Regular_ttf_len))
        {
            throw std::runtime_error("Failed to load system font!");
        }
        InitDefaultFontPreview();
    }

    void ResourceManager::InitDefaultFontPreview()
    {
        sf::Text text(m_Fonts.at("Default"), "Default", 24);
        std::string name = "Default";
        SetFontPreview(text, name);
    }

    void ResourceManager::GenerateFontPreview(AssetNode* node)
    {
        sf::Font font;
        if (!font.openFromFile(node->m_Path))
        {
            SS_DEBUG_LOG("[FONT] Failed to open font from path: " + node->m_Path.string());
            return;
        }
        sf::Text text(font, node->m_Name, 24);
        std::string name = node->m_Path.string();
        SetFontPreview(text, name);
    }

    void ResourceManager::SetFontPreview(sf::Text& text, std::string& name)
    {
        sf::FloatRect bounds = text.getLocalBounds();
        float padding = 10.0f;
        sf::Vector2u size = {
            static_cast<unsigned int>(bounds.size.x + padding),
            static_cast<unsigned int>(bounds.size.y + padding)
        };
        sf::Vector2f center = {
            static_cast<float>(size.x / 2),
            static_cast<float>(size.y / 2)
        };
        sf::Vector2f glyphCenter = {
            bounds.position.x + (bounds.size.x / 2.0f),
            bounds.position.y + (bounds.size.y / 2.0f)
        };
        if (size.x == 0 || size.y == 0) size = { 1, 1 };
        sf::RenderTexture fontViewer(size);
        text.setOrigin(glyphCenter);
        text.setPosition(center);
        fontViewer.clear(sf::Color::Transparent);
        fontViewer.draw(text);
        fontViewer.display();
        m_FontPreviews[name] = fontViewer.getTexture();
        m_FontPreviews[name].setSmooth(true);
    }

    sf::Texture& ResourceManager::GetFontPreview(std::string id)
    {
        auto found = m_FontPreviews.find(id);
        if (found != m_FontPreviews.end())
            return found->second;
        else
            return m_FontPreviews.at("Default");
    }

    void ResourceManager::ClearAllResources(bool clearDefaults)
    {
        m_Textures.clear();
        m_Fonts.clear();
        m_Animations.clear();
        m_SoundBuffers.clear();

        if (!clearDefaults)
            InitDefaultAssets();
    }

    void ResourceManager::PopulateLib(AssetNode* node, std::vector<AssetNode*>& library, const std::string& type)
    {
        for (auto& child : node->m_Children)
        {
            if (child->isDir) PopulateLib(child.get(), library, type);
            else if (child->m_Ext == type) library.push_back(child.get());
        }
    }

    const std::vector<AssetNode*> ResourceManager::GetAssetLibrary(const std::string& type)
    {
        std::vector<AssetNode*> library;
        PopulateLib(fileRoot.get(), library, type);
        return library;
    }

    sf::Texture& ResourceManager::GetThumbnail(AssetNode* node)
    {
        auto found = m_Thumbnails.find(node->m_Name);
        if (found == m_Thumbnails.end())
        {
            sf::Texture texture;
            if (!texture.loadFromFile(node->m_Path.string()))
            {
                texture = GetResource<sf::Texture>("empty");
            }
            texture.setSmooth(true);
            m_Thumbnails.emplace(node->m_Name, std::move(texture));
        }
        return m_Thumbnails.at(node->m_Name);
    }
}