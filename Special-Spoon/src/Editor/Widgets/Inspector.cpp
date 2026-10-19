#include "Editor/Editor.h"

namespace Spoon 
{
    void Editor::DrawInspector(sf::Time& tick)
    {
        ImGui::Begin("Special-Spoon Editor");
        if (m_ActiveScene)
        {
            ImGui::Text("Active Scene ID: %s", m_ActiveScene->ID.c_str());
            DrawEntityList();
            DrawComponentList();
        }
        else
        {
            ImGui::Text("Load/Create a scene to view and create entities!");
        }
        DrawMetrics(tick);
        ImGui::End();
    }
}