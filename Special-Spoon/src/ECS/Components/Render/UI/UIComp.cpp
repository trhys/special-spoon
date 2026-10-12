#include "UIComp.h"

namespace Spoon
{
    void UIComp::OnReflect()
    {
        ImGui::Checkbox("Active", &active);
        ImGui::Checkbox("Focused", &isFocused);
        ImGui::Checkbox("Hovered", &isHovered);

        if (ImGui::BeginCombo("Emitted Action", ActionRegistry::Get().GetName(emittedAction).c_str()))
        {
            for (const auto& [id, name] : ActionRegistry::Get().m_IDToName)
            {
                bool isSelected = (emittedAction == ActionRegistry::Get().GetAction(name));
                if (ImGui::Selectable(name.c_str(), isSelected))
                {
                    emittedAction = ActionRegistry::Get().GetAction(name);
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}