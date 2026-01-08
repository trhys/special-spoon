#pragma once

#include "Core/Core.h"
#include "ECS/Components/Render/SpriteComp.h"
#include "Editor/Utils/Viewport.h"

#include "SFML/Graphics.hpp"

namespace Spoon
{
	class TextureRectTool
	{
	public:
		void Run(SpriteComp& comp)
		{
            // Center this window only when it first appears
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::Begin("Texture Rect Editor"))
			{
                ImVec2 availableSize = ImGui::GetContentRegionAvail();
                availableSize.y -= 30;
                
                if (ImGui::BeginChild("##TRE Viewport", availableSize, ImGuiChildFlags_Borders))
                {
                    RenderViewport(m_Viewport);

                    sf::RectangleShape rectPreview;
                    rectPreview.setSize({ (float)m_Rect.size.x, (float)m_Rect.size.y });
                    rectPreview.setPosition({ (float)m_Rect.position.x, (float)m_Rect.position.y });
                    rectPreview.setFillColor(sf::Color::Transparent);
                    rectPreview.setOutlineColor(sf::Color::Red);
                    rectPreview.setOutlineThickness(1.0);

                    m_Viewport.target.clear(sf::Color(50, 50, 50));
                    m_Viewport.target.draw(sf::Sprite(comp.m_Sprite.getTexture()));
                    m_Viewport.target.draw(rectPreview);
                    m_Viewport.target.display();

                    ImGui::Image(m_Viewport.target);
                    ImVec2 viewportPos = ImGui::GetItemRectMin();

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && ImGui::IsItemHovered())
                    {
                        ImVec2 mousePos = ImGui::GetIO().MousePos;
                        sf::Vector2i relativeMouse(
                            static_cast<int>(mousePos.x - viewportPos.x),
                            static_cast<int>(mousePos.y - viewportPos.y)
                        );
                        sf::Vector2f worldMouse = m_Viewport.target.mapPixelToCoords(relativeMouse);
                        if (!m_Dragging)
                        {
                            m_DragStart = worldMouse;
                            m_Dragging = true;
                        }
                        if (m_Dragging)
                        {
                            m_Rect.position.x = std::min(m_DragStart.x, worldMouse.x);
                            m_Rect.position.y = std::min(m_DragStart.y, worldMouse.y);
                            m_Rect.size.x = std::abs(worldMouse.x - m_DragStart.x);
                            m_Rect.size.y = std::abs(worldMouse.y - m_DragStart.y);
                        }
                    }
                    else m_Dragging = false;

                    ImGui::EndChild();
                }
                if (ImGui::Button("Confirm", ImVec2(120, 0)))
                {
                    comp.SetTextureRect(m_Rect);
                    m_Rect = sf::IntRect();
                    m_Dragging = false;
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    m_Rect = sf::IntRect();
                    m_Dragging = false;
                    comp.ToggleGizmo();
                }
                ImGui::SameLine();
                if (ImGui::Button("Close", ImVec2(120, 0)))
                {
                    m_Rect = sf::IntRect();
                    m_Dragging = false;
                    comp.ToggleGizmo();
                }
            }
            ImGui::End();
		}

        void Shutdown()
        {
            m_Viewport.target = sf::RenderTexture();
        }

	private:
		Viewport m_Viewport;
        sf::IntRect m_Rect;
        sf::Vector2f m_DragStart;
        bool m_Dragging = false;
	};
}