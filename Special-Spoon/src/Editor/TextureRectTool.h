#pragma once

#include "Core/Core.h"
#include "ECS/Components/Render/SpriteComp.h"
#include "Viewport.h"

#include "SFML/Graphics.hpp"

namespace Spoon
{
	class TextureRectTool
	{
	public:
		void Run(SpriteComp& comp)
		{
            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			if (ImGui::Begin("Texture Rect Editor"))
			{
                static sf::IntRect rect;

                if (ImGui::BeginChild("TRE Viewport", ImVec2(640, 640), ImGuiChildFlags_Borders))
                {
                    RenderViewport(m_Viewport, m_Camera);

                    static sf::Vector2f drag;
                    static bool dragging = false;
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && ImGui::IsWindowHovered())
                    {
                        ImVec2 viewportPos = ImGui::GetCursorScreenPos();
                        ImVec2 mousePos = ImGui::GetIO().MousePos;
                        sf::Vector2i relativeMouse(
                            static_cast<int>(mousePos.x - viewportPos.x),
                            static_cast<int>(mousePos.y - viewportPos.y)
                        );
                        sf::Vector2f worldMouse = m_Viewport.mapPixelToCoords(relativeMouse);
                        if (!dragging)
                        {
                            drag = worldMouse;
                            dragging = true;
                        }
                        if (dragging)
                        {
                            rect.position.x = std::min(drag.x, worldMouse.x);
                            rect.position.y = std::min(drag.y, worldMouse.y);
                            rect.size.x = std::abs(worldMouse.x - drag.x);
                            rect.size.y = std::abs(worldMouse.y - drag.y);
                        }
                    }
                    else dragging = false;

                    sf::RectangleShape rectPreview;
                    rectPreview.setSize({ (float)rect.size.x, (float)rect.size.y });
                    rectPreview.setPosition({ (float)rect.position.x, (float)rect.position.y });
                    rectPreview.setFillColor(sf::Color::Transparent);
                    rectPreview.setOutlineColor(sf::Color::Red);
                    rectPreview.setOutlineThickness(1.0);

                    m_Viewport.clear(sf::Color(50, 50, 50));
                    m_Viewport.draw(sf::Sprite(comp.m_Sprite.getTexture()));
                    m_Viewport.draw(rectPreview);
                    m_Viewport.display();

                    ImGui::Image(m_Viewport);
                    ImGui::EndChild();
                }
                if (ImGui::Button("Confirm"))
                {
                    comp.SetTextureRect(rect);
                    comp.ToggleGizmo();
                }
            }
            ImGui::End();
		}

	private:
		sf::RenderTexture m_Viewport;
		sf::View m_Camera;
	};
}