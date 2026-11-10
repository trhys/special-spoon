#include "AIComp.h"
#include "Core/Application.h"
#include "Utils/Macros.h"

namespace Spoon
{
    void AIComponent::OnReflect()
    {
        ImGui::Text("Active Script: %s", m_ActiveScript.string().c_str());

		ImGui::SeparatorText("Available Scripts");
		auto scriptsPath = Application::Get().GetProjectManager().GetCurrentProjectPaths().Scripts();
        try {
            if (std::filesystem::create_directories(scriptsPath))
            {
                SS_DEBUG_LOG("[AI COMP] Created scripts directory: " + scriptsPath.string())
            }
            if (std::filesystem::exists(scriptsPath))
            {
                for (const auto& file : std::filesystem::directory_iterator(scriptsPath))
                {
                    if (!file.is_regular_file() || file.path().extension() != ".lua")
                        continue;
                    const auto& name = file.path().stem().string();
                    if (ImGui::Selectable(name.c_str()))
                    {
                        m_ActiveScript = file.path();
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            SS_DEBUG_LOG("[AI COMP] File system error: " + std::string(e.what()))
		}

    }
}