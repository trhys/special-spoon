#include "Editor/Editor.h"

namespace Spoon
{
    void Editor::DrawMetrics(sf::Time& tick)
    {
        if (!m_ShowMetrics)
            return;

        // display flags for each section
        static bool showSystemMetrics = true;
        static bool showDrawTimeMetrics = true;

        ImGui::SeparatorText("Performance Metrics");
        ImGui::Text("AVG FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Current FPS: %.1f", 1.f / tick.asSeconds());
        ImGui::Text("Draw Calls: %d", m_Services.renderer->GetDrawCalls());
        ImGui::SameLine(); HelpMarker("Number of draw calls submitted to the GPU per frame.\n This only counts draws called from the Renderer.\n"
                                        "External draws (e.g. ImGui) are not counted.");

        if (showSystemMetrics)
        {
            ImGui::SeparatorText("System Metrics"); ImGui::SameLine();
            HelpMarker("Time spent in each system's Update function.");
        
            ImGui::Text("Total Update Time: %.2f ms", m_Services.metrics->Get("Total_Update").Value<float>());
            ImGui::Separator();
            ImGui::Text("AI System Update Time: %.2f ms", m_Services.metrics->Get("AI_Update").Value<float>());
            const SystemManager& systemManager = *m_Services.systemManager;
            for (const auto& systems : systemManager.GetSystems())
            {
                std::string timeID = systems->GetDisplayName() + "_Update";
                ImGui::Text("%s: %.2f ms", timeID.c_str(), m_Services.metrics->Get(timeID).Value<float>());
            }
        }

        if (showDrawTimeMetrics)
        {
            ImGui::SeparatorText("Draw Time Metrics");
            ImGui::Text("Editor Draw Time: %.2f ms", m_Services.metrics->Get("Editor_Drawtime").Value<float>());
            ImGui::Text("Window Draw Time: %.2f ms", m_Services.metrics->Get("Window_Drawtime").Value<float>());
        }
    }
}