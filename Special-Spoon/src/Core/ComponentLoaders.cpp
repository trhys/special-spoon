#include "ComponentLoaders.h"
#include "ECS/ECS.h"
#include "EntityManager.h"
#include "ResourceManager.h"

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
        
        bool centered = comp.value("Centered", false);

        if (comp.contains("TextureRect"))
        {
            sf::IntRect rect(
                { comp["TextureRect"]["x"].get<int>(), comp["TextureRect"]["y"].get<int>() },
                { comp["TextureRect"]["width"].get<int>(), comp["TextureRect"]["height"].get<int>() }
            );
            manager.MakeComponent<SpriteComp>(id, asset, rect, centered);
        }
        else
        {
            manager.MakeComponent<SpriteComp>(id, asset, centered);
        }
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

    void LoadAnimationComponent(EntityManager& manager, UUID id, const json& comp)
    {
        std::string animationID = comp["AnimationID"].get<std::string>();
        
        if(comp.contains("AnimationMap"))
        {
            std::unordered_map<std::string, std::string> animationMap;
            for(auto& [key, value] : comp["AnimationMap"].items())
            {
                animationMap[key] = value.get<std::string>();
            }
            manager.MakeComponent<AnimationComp>(id, animationID, animationMap);
        }
        else 
        { 
            manager.MakeComponent<AnimationComp>(id, animationID); 
        }
    }

    void LoadStatusComponent(EntityManager& manager, UUID id, const json& comp)
    {
        bool is_Active = true;
        std::string currentState = "Idle";
        if(comp.contains("is_Active")) { is_Active = comp["is_Active"].get<bool>(); }
        if(comp.contains("CurrentState")) { currentState = comp["CurrentState"].get<std::string>(); }
        manager.MakeComponent<StatusComp>(id, is_Active, currentState);
    }

    void LoadBlinkComponent(EntityManager& manager, UUID id, const json& comp)
    {
        float interval = comp["Interval"].get<float>();
        manager.MakeComponent<BlinkComp>(id, interval);
    }

    void LoadInputComponent(EntityManager& manager, UUID id, const json& comp)
    {
        std::unordered_map<std::string, std::string> keyBindings;
        for(auto& [key, value] : comp["KeyBindings"].items())
        {
            keyBindings[key] = value.get<std::string>();
        }
        manager.MakeComponent<InputComp>(id, keyBindings);
    }

    void LoadStateActionComponent(EntityManager& manager, UUID id, const json& comp)
    {
        std::unordered_map<std::string, std::string> stateActions;
        for(auto& [actionTrigger, targetState] : comp["Actions"].items())
        {
            stateActions[actionTrigger] = targetState.get<std::string>();
        }
        manager.MakeComponent<StateActionComp>(id, stateActions);
    }

    void LoadRenderLayer(EntityManager& manager, UUID id, const json& comp)
    {
        int layer = comp.value("Layer", 1);
        manager.MakeComponent<RenderLayer>(id, layer);
    }

    void RegisterDefaultLoaders()
    {
        SS_DEBUG_LOG("[COMPONENT] Registering default component loaders...")
        ComponentLoaders::RegisterCompLoader("Transform", &LoadTransformComponent);
        ComponentLoaders::RegisterCompLoader("Sprite", &LoadSpriteComponent);
        ComponentLoaders::RegisterCompLoader("Text", &LoadTextComponent);
        ComponentLoaders::RegisterCompLoader("Animation", &LoadAnimationComponent);
        ComponentLoaders::RegisterCompLoader("Status", &LoadStatusComponent);
        ComponentLoaders::RegisterCompLoader("Blink", &LoadBlinkComponent);
        ComponentLoaders::RegisterCompLoader("Input", &LoadInputComponent);
        ComponentLoaders::RegisterCompLoader("StateAction", &LoadStateActionComponent);
        ComponentLoaders::RegisterCompLoader("RenderLayer", &LoadRenderLayer);
    }

    namespace
    {
        class RegisterDefaultLoadersHelper
        {
        public:
            RegisterDefaultLoadersHelper()
            {
                RegisterDefaultLoaders();
            }
    };
        static RegisterDefaultLoadersHelper s_RegisterDefaultLoadersHelper;
    }
}