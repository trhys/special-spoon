#include "Utils/Logger.h"
#include "Editor/Editor.h"

namespace Spoon
{
    void Editor::DrawLogger()
    {
        if (!m_ShowLogger)
            return;
        
        auto messages = Logger::Get().GetEditorMessages();
        ImGui::Begin("Log Console");
        for (const auto& message : messages)
        {
            ImGui::Text("%s", message.c_str());
        }
        ImGui::End();
    }
}