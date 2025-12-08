#include "ComponentLoaders.h"
#include "ECS/ECS.h"
#include "EntityManager.h"

namespace Spoon
{
    void LoadTransformComponent(EntityManager& manager, UUID id, const json& comp)
    {
        float x = comp["Position"]["x"].get<float>();
        float y = comp["Position"]["y"].get<float>();
        manager.MakeComponent<TransformComp>(id, sf::Vector2f(x, y));
    }

    void LoadSpriteComponent(EntityManager& manager, UUID id, const json& comp)
    {
        std::string textureID = comp["TextureID"].get<std::string>();
        sf::Texture& asset = ResourceManager::GetResource<sf::Texture>(textureID);
        manager.MakeComponent<SpriteComp>(id, asset);
    }

    void LoadTextComponent(EntityManager& manager, UUID id, const json& comp)
    {
        std::string fontID = comp["FontID"].get<std::string>();
        sf::Font& asset = ResourceManager::GetResource<sf::Font>(fontID);
        std::string content = comp["Content"].get<std::string>();
        if(comp.contains("CharacterSize"))
        {
            unsigned int charSize = comp["CharacterSize"].get<unsigned int>();
            manager.MakeComponent<TextComp>(id, asset, content, charSize);
        }
        else
        {
            manager.MakeComponent<TextComp>(id, asset, content);
        }
    }

    void RegisterDefaultLoaders()
    {
        ComponentLoader::RegisterLoader("Transform", &LoadTransformComponent);
        ComponentLoader::RegisterLoader("Sprite", &LoadSpriteComponent);
        ComponentLoader::RegisterLoader("Text", &LoadTextComponent);
    }

    static const bool registered = []() {
        RegisterDefaultLoaders();
        return true;
    }();
}