#include "AIComp.h"

namespace Spoon
{
    void AIComponent::OnReflect()
    {
        ImGui::Text("Active Script: %s", m_ActiveScript.string().c_str());
    }
}