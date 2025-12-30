#include "Viewport.h"
#include "SFML/Graphics.hpp"

namespace Spoon
{
    void RenderViewport(sf::RenderTexture& target, sf::View& camera)
    {
        // Resize/Zoom viewport if necessary
        sf::Vector2f viewportSize = ImGui::GetContentRegionAvail();
        sf::Vector2u viewport2u(
            std::max(1u, static_cast<unsigned int>(viewportSize.x)),
            std::max(1u, static_cast<unsigned int>(viewportSize.y)));
        if (target.getSize() != viewport2u)
            if (target.resize({ viewport2u }))
            {
                camera.setSize({ (float)viewport2u.x, (float)viewport2u.y });
                target.setView(camera);
            }
        if (ImGui::IsWindowHovered())
        {
            float scrollDelta = ImGui::GetIO().MouseWheel;
            if (scrollDelta != 0.0)
            {
                camera.zoom((scrollDelta > 0) ? 0.9 : 1.1);
                target.setView(camera);
            }
        }
        static sf::Vector2f mouseDrag;
        static bool dragging = false;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && ImGui::IsWindowHovered())
        {
            ImVec2 viewportPos = ImGui::GetCursorScreenPos();
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            sf::Vector2i relativeMouse(
                static_cast<int>(mousePos.x - viewportPos.x),
                static_cast<int>(mousePos.y - viewportPos.y)
            );
            sf::Vector2f worldMouse = target.mapPixelToCoords(relativeMouse);
            if (!dragging)
            {
                mouseDrag = worldMouse;
                dragging = true;
            }
            if (dragging)
            {
                camera.move(mouseDrag - worldMouse);
                mouseDrag = target.mapPixelToCoords(relativeMouse);
            }
            target.setView(camera);
        }
        else dragging = false;
    }
}