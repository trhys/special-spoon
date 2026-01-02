#pragma once

#include "ECS/Components/Component.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "SFML/Graphics/Text.hpp"

namespace Spoon
{
    struct TextComp : public ComponentBase<TextComp>
    {
        TextComp(sf::Font& asset = ResourceManager::Get().GetResource<sf::Font>("Default"), std::string text = "",
            unsigned int char_size = 30, sf::Color color = sf::Color::White, sf::Color outcolor = sf::Color::White, unsigned int olThickness = 0, bool centered = true)
            : ComponentBase::ComponentBase("Text"), m_Text(asset, text, char_size), isCentered(centered), iText(text),
             iCharSize(char_size), iColor(color), iOutColor(outcolor), iolThickness(olThickness)
        { 
            m_Text.setFillColor(color);
            m_Text.setOutlineColor(outcolor);
            m_Text.setOutlineThickness(olThickness);
            if (centered) { CenterOrigin(); }
        }

        sf::Text m_Text;
        bool isCentered;
        std::string iFontID;
        std::string iText;
        unsigned int iCharSize;
        unsigned int iolThickness;
        sf::Color iColor;
        sf::Color iOutColor;

        sf::Text& GetText() { return m_Text; }

        void SetText(std::string text) { m_Text.setString(text); }
        void SetFont(sf::Font& font) { m_Text.setFont(font); }
        void SetSize(unsigned int size) { m_Text.setCharacterSize(size); }
        void SetPosition(sf::Vector2f pos) { m_Text.setPosition(pos); }
        void SetColor(sf::Color color) { m_Text.setFillColor(color); }
        void CenterOrigin() 
        {
            if (isCentered)
            {
                sf::FloatRect bounds = m_Text.getLocalBounds(); 
                m_Text.setOrigin({ 
                    bounds.position.x + (bounds.size.x / 2.0f), 
                    bounds.position.y + (bounds.size.y / 2.0f) 
                });
            }
            else
            {
                m_Text.setOrigin({0.0, 0.0});
            }
            
        }
        void SetAlpha(float alpha)
        {
            sf::Color color = m_Text.getFillColor();
            color.a = static_cast<uint8_t>(alpha);
            m_Text.setFillColor(color);
        }

        void OnReflect() override
        {
            std::string text = m_Text.getString();
            static char newTextBuf[64];
            
            ImGui::Text("Text: %s", text.c_str());
            if (ImGui::Button("Edit String"))
            {
                strncpy(newTextBuf, text.c_str(), sizeof(newTextBuf));
                ImGui::OpenPopup("Edit Text String");
            }

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Edit Text String"))
            {
                bool changedString = ImGui::InputText("##New String", newTextBuf, IM_ARRAYSIZE(newTextBuf), ImGuiInputTextFlags_EnterReturnsTrue);
                
                if (ImGui::Button("Submit") || changedString)
                {
                    iText = newTextBuf;
                    m_Text.setString(newTextBuf);
                    CenterOrigin();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginChild("Font Explorer", ImVec2(0, 200), ImGuiChildFlags_Borders))
            {   
                for (const auto& [id, font] : ResourceManager::Get().GetFonts())
                {
                    ImGui::PushID(id.c_str());
                    ImGui::Image(ResourceManager::Get().GetFontPreview(id));
                    ImGui::SameLine();
                    if (ImGui::Selectable(id.c_str()))
                    {
                        m_Text.setFont(font);
                        iFontID = id;
                    }
                    ImGui::PopID();
                }
                ImGui::EndChild();
            }

            int charSize = static_cast<int>(m_Text.getCharacterSize());
            ImGui::SeparatorText("Character Size");
            if (ImGui::SliderInt("##CharSize", &charSize, 1, 100))
            {
                iCharSize = charSize;
                SetSize(static_cast<unsigned int>(charSize));
                CenterOrigin();
            }

            ImGui::SeparatorText("Center Origin");
            if(ImGui::Checkbox("Centered", &isCentered)) CenterOrigin();

            ImGui::SeparatorText("Fill Color");
            sf::Color m_Color = m_Text.getFillColor();
            float color[4] = {
                m_Color.r / 255.0f,
                m_Color.g / 255.0f,
                m_Color.b / 255.0f,
                m_Color.a / 255.0f
            };
            if (ImGui::ColorEdit4("Fill Color", color))
            {
                m_Color.r = static_cast<std::uint8_t>(color[0] * 255.0f);
                m_Color.g = static_cast<std::uint8_t>(color[1] * 255.0f);
                m_Color.b = static_cast<std::uint8_t>(color[2] * 255.0f);
                m_Color.a = static_cast<std::uint8_t>(color[3] * 255.0f);
                iColor = m_Color;
                SetColor(m_Color);
            }

            ImGui::SeparatorText("Outline");
            sf::Color outline_Color = m_Text.getOutlineColor();
            float OLcolor[4] = {
                outline_Color.r / 255.0f,
                outline_Color.g / 255.0f,
                outline_Color.b / 255.0f,
                outline_Color.a / 255.0f
            };
            if (ImGui::ColorEdit4("Outline Color", OLcolor))
            {
                outline_Color.r = static_cast<std::uint8_t>(OLcolor[0] * 255.0f);
                outline_Color.g = static_cast<std::uint8_t>(OLcolor[1] * 255.0f);
                outline_Color.b = static_cast<std::uint8_t>(OLcolor[2] * 255.0f);
                outline_Color.a = static_cast<std::uint8_t>(OLcolor[3] * 255.0f);
                iOutColor = outline_Color;
                m_Text.setOutlineColor(outline_Color);
            }

            float olThickness = m_Text.getOutlineThickness();
            if (ImGui::SliderFloat("Outline Thickness", &olThickness, 0.0f, 50.0f))
            {
                iolThickness = olThickness;
                m_Text.setOutlineThickness(olThickness);
                CenterOrigin();
            }
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextComp, iFontID, isCentered, iCharSize, iColor, iText, iolThickness, iOutColor)
}