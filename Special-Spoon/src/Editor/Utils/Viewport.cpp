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
            viewport.state.viewportPos = viewportPos;
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            sf::Vector2i relativeMouse(
                static_cast<int>(mousePos.x - viewportPos.x),
                static_cast<int>(mousePos.y - viewportPos.y)
            );
            if (!viewport.state.dragging)
            {
                viewport.state.mouseDrag = viewport.target.mapPixelToCoords(relativeMouse);
                viewport.state.dragging = true;
            }
            else if (viewport.state.dragging)
            {
                sf::Vector2f currentMouse = viewport.target.mapPixelToCoords(relativeMouse);
                viewport.camera.move(viewport.state.mouseDrag - currentMouse);
                viewport.target.setView(viewport.camera);
            }
        }
        else viewport.state.dragging = false;
    }

    ImVec2 GetAspectRatio(sf::Texture& texture)
    {
        ImVec2 space = ImGui::GetContentRegionAvail();
        sf::Vector2u texSize = texture.getSize();
        if (texSize.x == 0 || texSize.y == 0)
            return ImVec2(32, 32);
        float aspect = (float)texSize.x / (float)texSize.y;
        float displayWidth = space.x;
        float displayHeight = space.x / aspect;
        if (displayHeight > space.y) 
        {
            displayHeight = space.y;
            displayWidth = space.y * aspect;
        }
        return ImVec2(displayWidth, displayHeight);
    }
}