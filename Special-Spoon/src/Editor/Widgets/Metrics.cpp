#include "Editor/Editor.h"

namespace Spoon
{
    void Editor::DrawMetrics(sf::Time& tick)
    {
        if (!m_ShowMetrics)
            return;

        // Displays metrics and debug info
        ImGui::SeparatorText("Performance Metrics");
        ImGui::Text("AVG FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Current FPS: %.1f", 1.f / tick.asSeconds());
        ImGui::Text("Draw Calls: %d", m_Services.renderer->GetDrawCalls());
        ImGui::SameLine(); HelpMarker("Number of draw calls submitted to the GPU per frame.\n This only counts draws called from the Renderer.\n"
                                        "External draws (e.g. ImGui) are not counted.");
        ImGui::Text("Draw Time: %.2f ms", m_Services.renderer->GetDrawTime());
        ImGui::SameLine(); HelpMarker("Time spent in the Renderer drawing all renderables this frame.");
    }
}