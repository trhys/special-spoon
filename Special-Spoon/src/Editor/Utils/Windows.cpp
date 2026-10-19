#include "Windows.h"
#include "Imgui/imgui.h"

namespace Spoon
{
    void CenterNext()
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    }
}