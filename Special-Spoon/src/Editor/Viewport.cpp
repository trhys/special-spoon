#include "Viewport.h"

namespace Spoon
{
    void RenderViewport(Viewport& viewport)
    {
        // Resize/Zoom viewport if necessary
        sf::Vector2f viewportSize = ImGui::GetContentRegionAvail();
        sf::Vector2u viewport2u(
            std::max(1u, static_cast<unsigned int>(viewportSize.x)),
            std::max(1u, static_cast<unsigned int>(viewportSize.y)));
        if (viewport.target.getSize() != viewport2u)
            if (viewport.target.resize({ viewport2u }))
            {
                viewport.camera.setSize({ (float)viewport2u.x, (float)viewport2u.y });
                viewport.target.setView(viewport.camera);
            }
        if (ImGui::IsWindowHovered())
        {
            float scrollDelta = ImGui::GetIO().MouseWheel;
            if (scrollDelta != 0.0)
            {
                viewport.camera.zoom((scrollDelta > 0) ? 0.9 : 1.1);
                viewport.target.setView(viewport.camera);
            }
        }
        
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && ImGui::IsWindowHovered())
        {
            ImVec2 viewportPos = ImGui::GetCursorScreenPos();
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            sf::Vector2i relativeMouse(
                static_cast<int>(mousePos.x - viewportPos.x),
                static_cast<int>(mousePos.y - viewportPos.y)
            );
            sf::Vector2f worldMouse = viewport.target.mapPixelToCoords(relativeMouse);
            if (!viewport.state.dragging)
            {
                viewport.state.mouseDrag = worldMouse;
                viewport.state.dragging = true;
            }
            if (viewport.state.dragging)
            {
                viewport.camera.move(viewport.state.mouseDrag - worldMouse);
                viewport.state.mouseDrag = viewport.target.mapPixelToCoords(relativeMouse);
            }
            viewport.target.setView(viewport.camera);
        }
        else viewport.state.dragging = false;
    }
}