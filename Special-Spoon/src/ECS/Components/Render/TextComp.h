#pragma once

#include "ECS/Components/Component.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "SFML/Graphics/Text.hpp"

namespace Spoon
{
    struct TextComp : public ComponentBase<TextComp>
    {
        TextComp(sf::Font& asset = ResourceManager::Get().GetResource<sf::Font>("Default"), std::string id = "Default", std::string text = "",
            unsigned int char_size = 30, sf::Color color = sf::Color::White, sf::Color outcolor = sf::Color::White, unsigned int olThickness = 0, bool centered = true)
            : ComponentBase::ComponentBase(Name), m_Text(asset, text, char_size), isCentered(centered), iText(text),
			iCharSize(char_size), iColor(color), iOutColor(outcolor), iolThickness(olThickness), iFontID(id)
        { 
            m_Text.setFillColor(color);
            m_Text.setOutlineColor(outcolor);
            m_Text.setOutlineThickness(olThickness);
            if (centered) { CenterOrigin(); }
        }

        static constexpr const char* Name = "Text";
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
        void SetRotation(float rot) { m_Text.setRotation(sf::degrees(rot)); }
        void SetColor(sf::Color color) { m_Text.setFillColor(color); }
        void CenterOrigin();
        void SetAlpha(float alpha);

        void OnReflect() override;

		// renderable interface
		void PreRender(EntityManager& manager, UUID id) override;
		void Render(sf::RenderTarget& target, sf::RenderStates& states) override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextComp, iFontID, isCentered, iCharSize, iColor, iText, iolThickness, iOutColor)
}
