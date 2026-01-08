#include "ComponentLoaders.h"
#include "Core/EntityManager/EntityManager.h"
#include "ECS/ECS.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "Core/Serialization/JsonTypeDefs.h"

namespace Spoon
{
    void LoadTransformComponent(EntityManager& manager, UUID id, const json& comp)
    {
        sf::Vector2f pos = comp["iPos"].get<sf::Vector2f>();
        sf::Vector2f scale = comp["iScale"].get<sf::Vector2f>();
        float rot = comp["iRot"].get<float>();
        manager.MakeComponent<TransformComp>(id, TransformComp::Name, pos, scale, rot);
    }

    void LoadSpriteComponent(EntityManager& manager, UUID id, const json& comp)
    {
        auto scomp = comp.get<SpriteComp>();
        sf::Texture& asset = ResourceManager::Get().GetResource<sf::Texture>(scomp.m_TextureID);
        manager.MakeComponent<SpriteComp>(id, SpriteComp::Name, asset, scomp.m_TextureRect, scomp.isCentered, scomp.m_TextureID);
    }

    void LoadTextComponent(EntityManager& manager, UUID id, const json& comp)
    {
        auto tcomp = comp.get<TextComp>();
        sf::Font& asset = ResourceManager::Get().GetResource<sf::Font>(tcomp.iFontID);
        manager.MakeComponent<TextComp>(id, TextComp::Name, asset, tcomp.iFontID, tcomp.iText, tcomp.iCharSize,
             tcomp.iColor, tcomp.iOutColor, tcomp.iolThickness, tcomp.isCentered);
    }

    void LoadAnimationComponent(EntityManager& manager, UUID id, const json& comp)
    {
        auto acomp = comp.get<AnimationComp>();
        manager.MakeComponent<AnimationComp>(id, AnimationComp::Name, acomp.iAnimationID, acomp.m_AnimationMap);
    }

    void LoadStatusComponent(EntityManager& manager, UUID id, const json& comp)
    {
        auto status = comp.get<StatusComp>();
        manager.MakeComponent<StatusComp>(id, StatusComp::Name, status.is_Active, status.m_CurrentState);
    }

    void LoadBlinkComponent(EntityManager& manager, UUID id, const json& comp)
    {
        auto blink = comp.get<BlinkComp>();
        manager.MakeComponent<BlinkComp>(id, BlinkComp::Name, blink.m_Blinkrate);
    }

    void LoadFadeComponent(EntityManager& manager, UUID id, const json& comp)
    {
        auto fade = comp.get<FadeComp>();
        manager.MakeComponent<FadeComp>(id, FadeComp::Name, fade.m_FadeRate);
    }

    void LoadInputComponent(EntityManager& manager, UUID id, const json& comp)
    {
        // todo: #45 fix this
         auto input = comp.get<InputComp>();
         manager.MakeComponent<InputComp>(id, InputComp::Name, input.m_KeyBindings);
    }

    void LoadStateActionComponent(EntityManager& manager, UUID id, const json& comp)
    {
        auto component = comp.get<StateActionComp>();
        manager.MakeComponent<StateActionComp>(id, StateActionComp::Name, component.m_Actions);
    }

    void LoadRenderLayer(EntityManager& manager, UUID id, const json& comp)
    {
        auto renderlayer = comp.get<RenderLayer>();
        manager.MakeComponent<RenderLayer>(id, RenderLayer::Name, renderlayer.m_Layer);
    }

    void LoadPhysicsComponent(EntityManager& manager, UUID id, const json& comp)
    {
        manager.MakeComponent<PhysicsComp>(id, PhysicsComp::Name);
    }

    void LoadColorComponent(EntityManager& manager, UUID id, const json& comp)
    {
        sf::Color color = comp.get<sf::Color>();
        manager.MakeComponent<ColorComp>(id, ColorComp::Name, color);
    }

    void RegisterDefaultLoaders()
    {
        SS_DEBUG_LOG("[COMPONENT] Registering default component loaders...")
        ComponentLoaders::RegisterCompLoader(TransformComp::Name, &LoadTransformComponent);
        ComponentLoaders::RegisterCompLoader(SpriteComp::Name, &LoadSpriteComponent);
        ComponentLoaders::RegisterCompLoader(TextComp::Name, &LoadTextComponent);
        ComponentLoaders::RegisterCompLoader(AnimationComp::Name, &LoadAnimationComponent);
        ComponentLoaders::RegisterCompLoader(StatusComp::Name, &LoadStatusComponent);
        ComponentLoaders::RegisterCompLoader(BlinkComp::Name, &LoadBlinkComponent);
        ComponentLoaders::RegisterCompLoader(FadeComp::Name, &LoadFadeComponent);
        ComponentLoaders::RegisterCompLoader(InputComp::Name, &LoadInputComponent);
        ComponentLoaders::RegisterCompLoader(StateActionComp::Name, &LoadStateActionComponent);
        ComponentLoaders::RegisterCompLoader(RenderLayer::Name, &LoadRenderLayer);
        ComponentLoaders::RegisterCompLoader(PhysicsComp::Name, &LoadPhysicsComponent);
        ComponentLoaders::RegisterCompLoader(ColorComp::Name, &LoadColorComponent);
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