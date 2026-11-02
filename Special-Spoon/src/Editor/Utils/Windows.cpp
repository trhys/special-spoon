#include "Windows.h"
#include "Viewport.h"
#include "Core/ResourceManager/ResourceManager.h"

#include "Imgui/imgui.h"

namespace Spoon
{
    void CenterNext()
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    }

    void DrawTextureBrowser(AssetNode*& selectedNode)
    {
        ImGui::Text("Select a texture");
        ImGui::Separator();
        static std::filesystem::path selectedPath;
        const auto library = ResourceManager::Get().GetAssetLibrary("Image file");
        for (auto* node : library)
        {
            if (node->m_Path == selectedPath)
            {
                selectedNode = node;
                break;
            }
        }
        if (ImGui::BeginChild("Texture Explorer", ImVec2(0, 200), ImGuiChildFlags_Borders))
        {
            for (const auto& node : library)
            {
                ImGui::PushID(node->m_Name.c_str());
                if (ImGui::ImageButton(node->m_Name.c_str(), ResourceManager::Get().GetThumbnail(node).getNativeHandle(), ImVec2(128, 128)))
                {
                    selectedNode = node;
                    selectedPath = node->m_Path;
                }
                ImGui::SameLine();
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        if (ImGui::BeginChild("Preview", ImVec2(0, 600), ImGuiChildFlags_Borders))
        {
            if (selectedNode)
            {
                ImGui::Text("Selected texture id: %s", selectedNode->m_Name.c_str());

                // Get aspect ratio for texture preview
                sf::Texture& texture = ResourceManager::Get().GetThumbnail(selectedNode);
                ImGui::Image(texture, GetAspectRatio(texture));
            }
            else
            {
                ImGui::Text("No texture selected");
            }
        }
        ImGui::EndChild();
    }
}