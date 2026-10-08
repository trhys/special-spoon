#include "SpriteComp.h"
#include "Core/Application.h"

namespace Spoon
{
    void SpriteComp::CenterOrigin()
    {
        sf::Vector2f size = (m_TextureRect.size.x != 0 && m_TextureRect.size.y != 0)
            ? sf::Vector2f(m_TextureRect.size)
            : m_Sprite.getLocalBounds().size;

        if (isCentered)
        {
            m_Sprite.setOrigin({ (size.x / 2.0f), (size.y / 2.0f) });
        }
        else
        {
            m_Sprite.setOrigin({ 0.0, 0.0 });
        }
    }
    void SpriteComp::SetAlpha(float alpha)
    {
        sf::Color color = m_Sprite.getColor();
        color.a = static_cast<uint8_t>(alpha);
        SetColor(color);
    }

    void SpriteComp::PreRender(EntityManager& manager, UUID id) 
    {
        auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
        auto& colorArray = manager.GetArray<ColorComp>(ColorComp::Name);

        // sync transform - the transform is logical space, the sprite is
        // presentation space (see Core/Renderer/WorldProjection.h)
        if(transformArray.m_IdToIndex.count(id))
        {
            TransformComp& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);
            m_LogicalPosition = transform.GetPosition();
            m_Sprite.setPosition(m_LogicalPosition);
            m_Sprite.setScale(transform.GetScale());
            m_Sprite.setRotation(transform.m_Transform.getRotation());

            // when projected, keep the artwork upright and only move it: the
            // sprite's logical rect is treated as its ground footprint and the
            // sprite's bottom-center (feet) is placed on the projected center
            // of that footprint. nothing is written back to the transform.
            const WorldProjection& projection = Application::Get().GetRenderer().GetProjection();
            if (projection.IsActive())
            {
                const sf::FloatRect logicalBounds = m_Sprite.getGlobalBounds();
                const sf::Vector2f footprintCenter = logicalBounds.getCenter();
                const sf::Vector2f feet{
                    footprintCenter.x,
                    logicalBounds.position.y + logicalBounds.size.y
                };
                const sf::Vector2f projectedFeet = projection.LogicalToScreen(footprintCenter);
                m_Sprite.setPosition(m_LogicalPosition + (projectedFeet - feet));
            }
        }
        else
        {
            m_LogicalPosition = m_Sprite.getPosition();
        }

        // apply color comp if it exists
        if(colorArray.m_IdToIndex.count(id)) 
        {
            ColorComp& color = manager.GetComponent<ColorComp>(id, ColorComp::Name);
            m_Sprite.setColor(color.m_Color);
        }
    }

    void SpriteComp::OnReflect()
    {
        ImGui::Text("Texture ID: %s", m_TextureID.c_str());

        if (ImGui::BeginChild("Texture Explorer", ImVec2(0, 200), ImGuiChildFlags_Borders))
        {
            for (const auto& [id, texture] : ResourceManager::Get().GetTextures())
            {
                if (ImGui::ImageButton(id.c_str(), texture, sf::Vector2f(64, 64)))
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

        if (ImGui::Button(!ActiveGizmo() ? "Set Texture Rect" : "Cancel"))
        {
            ToggleGizmo();
        }
        if (ActiveGizmo())
        {
            Application::Get().GetEditor().EditTextureRect(*this);
        }

        ImGui::SeparatorText("Center Origin");
        if (ImGui::Checkbox("Centered", &isCentered)) CenterOrigin();

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
}
