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
            sf::Vector2f size = (m_TextureRect.size.x != 0 && m_TextureRect.size.y != 0) 
                ? sf::Vector2f(m_TextureRect.size) 
                : m_Sprite.getLocalBounds().size;

            if (isCentered)
            {
                m_Sprite.setOrigin({(size.x / 2.0f), (size.y / 2.0f)});
            }
            else
            {
                m_Sprite.setOrigin({0.0, 0.0});
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

            if(ImGui::BeginChild("Texture Explorer", ImVec2(0, 200), ImGuiChildFlags_Borders))
            {
                for(const auto& [id, texture] : ResourceManager::Get().GetTextures())
                {
                    if(ImGui::ImageButton(id.c_str(), texture, sf::Vector2f(64, 64)))
                    {
                        m_Sprite.setTexture(texture, true);
                        m_TextureID = id;
                        m_TextureRect = m_Sprite.getTextureRect();
                        CenterOrigin();
                    }
                    ImGui::SameLine();
                }
                ImGui::EndChild();
            }

            ImGui::SeparatorText("Center Origin");
            if(ImGui::Checkbox("Centered", &isCentered)) CenterOrigin();

            ImGui::SeparatorText("Color Selector");
            sf::Color m_Color = m_Sprite.getColor();
            float color[4] = {
                m_Color.r / 255.0f,
                m_Color.g / 255.0f,
                m_Color.b / 255.0f,
                m_Color.a / 255.0f
            };
            if (ImGui::ColorEdit4("Color", color))
            {
                m_Color.r = static_cast<std::uint8_t>(color[0] * 255.0f);
                m_Color.g = static_cast<std::uint8_t>(color[1] * 255.0f);
                m_Color.b = static_cast<std::uint8_t>(color[2] * 255.0f);
                m_Color.a = static_cast<std::uint8_t>(color[3] * 255.0f);
                SetColor(m_Color);
            }
        }
    };
}