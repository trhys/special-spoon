#pragma once

#include "ECS/Components/Component.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "SFML/Graphics/Sprite.hpp"

namespace Spoon
{
    struct SpriteComp : public ComponentBase<SpriteComp>
    {
        SpriteComp(sf::Texture& asset = ResourceManager::Get().GetResource<sf::Texture>("empty"), const sf::IntRect& rect = sf::IntRect(),
            bool centered = false, const std::string& textureID = "empty")
            : ComponentBase::ComponentBase(Name), m_Sprite(asset), m_TextureRect(rect), isCentered(centered), m_TextureID(textureID)
        {
            m_Sprite.setTextureRect(rect);
            if (centered) { CenterOrigin(); }
        }

        static constexpr const char* Name = "Sprite";

        // Core members
        sf::Sprite m_Sprite;
        sf::IntRect m_TextureRect;
        std::string m_TextureID;
        bool isCentered;

        // Core getters
        sf::Vector2f GetPosition() { return m_Sprite.getPosition(); }
        sf::FloatRect GetBoundingBox() { return m_Sprite.getGlobalBounds(); }

        // Core setters
        void SetTextureRect(const sf::IntRect& rect) { m_Sprite.setTextureRect(rect); m_TextureRect = rect; }
        void SetColor(sf::Color color) { m_Sprite.setColor(color); }
        void SetScale(sf::Vector2f scale) { m_Sprite.setScale(scale); }
        void SetPosition(sf::Vector2f pos) { m_Sprite.setPosition(pos); }
        void SetRotation(float rot) { m_Sprite.setRotation(sf::degrees(rot)); }
        void CenterOrigin();
        void SetAlpha(float alpha);

        // Editor methods
        void OnReflect() override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpriteComp, m_TextureID, isCentered, m_TextureRect)
}
