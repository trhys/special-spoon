#include "AnimationTool.h"
#include "Editor/Utils/Windows.h"
#include "Utils/Macros.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include <fstream>

namespace Spoon
{
    void AnimationTool::SaveAnimationData(const AnimationData& data, const std::filesystem::path& animationsDir)
    {
        std::filesystem::path savePath = animationsDir / (data.ID + ".json");
        std::ofstream outFile(savePath);
        if (outFile.is_open())
        {
            json animJson;
            to_json(animJson, data);
            outFile << animJson.dump(4);
            outFile.close();
        } else {
            SS_DEBUG_LOG("[ANIMATION TOOL] Failed to save animation data to: " + savePath.string())
            return;
        }
        SS_DEBUG_LOG("[ANIMATION TOOL] Successfully saved animation data to: " + savePath.string())
    }

    void LoadAnimationData(const std::filesystem::path& filePath, AnimationData& data)
    {
        std::ifstream inFile(filePath);
        if (inFile.is_open())
        {
            json animJson;
            inFile >> animJson;
            from_json(animJson, data);
            inFile.close();
        } else {
            SS_DEBUG_LOG("[ANIMATION TOOL] Failed to load animation data from: " + filePath.string())
            return;
        }
        SS_DEBUG_LOG("[ANIMATION TOOL] Successfully loaded animation data from: " + filePath.string())
    }

    // file dialog for selecting animation JSON files
    void DrawAnimationFileBrowser(std::filesystem::path& selectedJsonPath)
    {
        ImGuiFileDialog::Instance()->OpenDialog("SelectAnimationFile", "Select Animation JSON", ".json");
        if (ImGuiFileDialog::Instance()->Display("SelectAnimationFile"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                selectedJsonPath = ImGuiFileDialog::Instance()->GetFilePathName();
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void AnimationTool::DrawCreateNew()
    {
        if (!showCreate)
            return;
        const char* selectorPopup = "Texture Selector";
        if (!ImGui::IsPopupOpen(selectorPopup))
            ImGui::OpenPopup(selectorPopup);

        static char newAnimDatabuf[64];

        CenterNext();
        if (ImGui::BeginPopupModal(selectorPopup))
        {
            static AssetNode* selectedNode = nullptr;
            DrawTextureBrowser(selectedNode);
            if (ImGui::InputText("Animation ID", newAnimDatabuf, IM_ARRAYSIZE(newAnimDatabuf), ImGuiInputTextFlags_CharsNoBlank)) {}
            bool bufferValid = newAnimDatabuf[0] != '\0';
            ImGui::BeginDisabled(!bufferValid || !selectedNode);
            if (ImGui::Button("Confirm"))
            {
                std::string selectedID = selectedNode->m_Name;
                if (!ResourceManager::Get().GetTextures().count(selectedID))
                {
                    ResourceManager::Get().LoadResource<sf::Texture>(selectedNode->m_Name, selectedNode->m_Path);
                }
                AnimationData data;
                data.ID = newAnimDatabuf;
                data.textureID = selectedID;
                ResourceManager::Get().LoadAnimationData(data.ID, data);
                Open(ResourceManager::Get().GetAnimationData(data.ID));

                // save animation data to the animations directory
                SaveAnimationData(data, m_AnimationsDir);
                
                // clear input buffer and selection
                newAnimDatabuf[0] = '\0';
                selectedNode = nullptr;
                showCreate = false;
                editTool = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                // clear input buffer and selection
                newAnimDatabuf[0] = '\0';
                selectedNode = nullptr;
                showCreate = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void AnimationTool::DrawLoadModal()
    {
        if (!showLoad)
            return;
        if (!ImGui::IsPopupOpen("Open Animation Data"))
            ImGui::OpenPopup("Open Animation Data");

        CenterNext();
        if (ImGui::BeginPopupModal("Open Animation Data"))
        {
            // list all animation files in the animations directory
            for (const auto& entry : std::filesystem::directory_iterator(m_AnimationsDir))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".json")
                {
                    std::string fileName = entry.path().stem().string();
                    if (ImGui::Selectable(fileName.c_str()))
                    {
                        // load the selected animation data and open it in the editor
                        AnimationData data;
                        LoadAnimationData(entry.path(), data);
                        if (ResourceManager::Get().LoadAnimationData(data.ID, data))
                        {
                            Open(ResourceManager::Get().GetAnimationData(data.ID));
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
            }
            if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    void AnimationTool::DrawImportModal()
    {
        if (!showImport)
            return;
        if (!ImGui::IsPopupOpen("Import Animation Data"))
            ImGui::OpenPopup("Import Animation Data");

        CenterNext();
        if (ImGui::BeginPopupModal("Import Animation Data"))
        {
            // select texture
            static AssetNode* selectedNode = nullptr;
            DrawTextureBrowser(selectedNode);

            // select animation JSON file
            static std::filesystem::path selectedJsonPath;
            DrawAnimationFileBrowser(selectedJsonPath);

            if (ImGui::Button("Import"))
            {
                if (selectedNode && !selectedJsonPath.empty())
                {
                    ImportJSON(selectedNode, selectedJsonPath);
                }
                showImport = false;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Cancel")) 
            {
                showImport = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}