#pragma once

#include "ECS/Components/Component.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "SFML/Graphics/Sprite.hpp"

namespace Spoon
{
    struct SpriteComp : public ComponentBase<SpriteComp>
    {
        SpriteComp(sf::Texture& asset = ResourceManager::Get().GetResource<sf::Texture>("empty"), bool centered = false, std::string textureID = "empty")
        : ComponentBase::ComponentBase("SpriteComp"), m_Sprite(asset), isCentered(centered), m_TextureID(textureID) 
        { 
            if (centered) { CenterOrigin(); } 
        }

        SpriteComp(sf::Texture& asset, const sf::IntRect& rect, bool centered, std::string& textureID) 
        : ComponentBase::ComponentBase("SpriteComp"), m_Sprite(asset), m_TextureRect(rect), isCentered(centered), m_TextureID(textureID)
        {
            m_Sprite.setTextureRect(rect);
            if (centered) { CenterOrigin(); }
        }
                
        sf::Sprite m_Sprite;
        sf::IntRect m_TextureRect;
        std::string m_TextureID; // Really only for the inspector - no practical use otherwise
        bool isCentered;

        sf::Vector2f GetPosition() { return m_Sprite.getPosition(); }
        sf::FloatRect GetBoundingBox() { return m_Sprite.getGlobalBounds(); }

        void SetTextureRect(const sf::IntRect& rect) { m_Sprite.setTextureRect(rect); }
        void SetColor(sf::Color color) { m_Sprite.setColor(color); }
        void SetScale(sf::Vector2f scale) { m_Sprite.setScale(scale); }
        void SetPosition(sf::Vector2f pos) { m_Sprite.setPosition(pos); }
        void CenterOrigin() 
        {
            if (!isCentered)
            {
                sf::FloatRect bounds = m_Sprite.getLocalBounds(); 
                m_Sprite.setOrigin({ bounds.size.x / 2.0f, bounds.size.y / 2.0f });
            }
            else
            {
                m_Sprite.setOrigin({ 0.0, 0.0 });
            }
        }
        void SetAlpha(float alpha)
        {
            sf::Color color = m_Sprite.getColor();
            color.a = static_cast<uint8_t>(alpha);
            SetColor(color);
        }

        void OnReflect() override
        {
            ImGui::Text("Texture ID: %s", m_TextureID.c_str());

            ImGui::BeginChild("Texture Explorer");
            for(const auto& [id, texture] : ResourceManager::Get().GetTextures())
            {
                if(ImGui::ImageButton(id.c_str(), texture, sf::Vector2f(64, 64)))
                {
                    m_Sprite.setTexture(texture, true);
                }
            }

            ImGui::SeparatorText("Center Origin");
            if(ImGui::Checkbox("Centered", &isCentered)) CenterOrigin();
            ImGui::EndChild();
        }
    };
}