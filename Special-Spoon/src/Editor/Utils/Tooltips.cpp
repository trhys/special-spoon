#include "Tooltips.h"
#include "Core/Registers/ActionRegistry.h"

#include "Imgui/imgui.h"

namespace Spoon
{
	void ErrorMessage(const std::string& message)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", message.c_str());
	}

    // ui helper for available actions block
    void AvailableActionsList(uint32_t& selectedAction, const std::string& listBoxID)
    {
        ImGui::BeginChild("Available Actions", ImVec2(200, 300), true);
        if (ImGui::BeginListBox(listBoxID.c_str(), ImVec2(-FLT_MIN, -FLT_MIN)))
        {
            for (const auto& [name, id] : ActionRegistry::Get().m_NameToID)
            {
                const char* actionName = name.c_str();
                ImGui::PushID(actionName);
                const bool is_selected = (selectedAction == id);
                if (ImGui::Selectable(actionName, is_selected))
                {
                    selectedAction = id;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
                ImGui::PopID();
            }
            ImGui::EndListBox();
        }
        ImGui::EndChild();
    }
}