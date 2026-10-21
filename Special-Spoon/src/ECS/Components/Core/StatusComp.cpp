#include "StatusComp.h"
#include "Core/Registers/StateRegistry.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
    void StatusComp::ApplyState(StateType newState)
    {
        m_CurrentState = newState;
        resolvedThisFrame = true;
    }

    void StatusComp::OnReflect()
    {
        ImGui::TextDisabled("Current state: %s", StateRegistry::Get().GetName(m_CurrentState).c_str());
        ImGui::Checkbox("Is Active", &is_Active);

        ImGui::TextDisabled("Resolved this frame: %s", resolvedThisFrame ? "true" : "false");
    }
}