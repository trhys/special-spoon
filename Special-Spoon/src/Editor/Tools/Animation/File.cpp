#include "AnimationTool.h"
#include "Core/Application.h"
#include "Editor/Utils/Windows.h"
#include "Utils/Macros.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include <fstream>

namespace Spoon
{
    bool AnimationTool::SaveAnimationData(const AnimationData& data, const std::filesystem::path& animationsDir)
    {
        // validate animations directory
        try 
        {
            if (std::filesystem::create_directories(animationsDir))
            {
                SS_DEBUG_LOG("[ANIMATION TOOL] Created animations directory: " + animationsDir.string())
            }
        
            std::filesystem::path savePath = animationsDir / (data.ID + ".json");
            std::string jsonStr;
            try {
                json animJson;
                to_json(animJson, data);
                jsonStr = animJson.dump(4);
            } catch (const json::exception& e) {
                SS_DEBUG_LOG("[ANIMATION TOOL] Failed to serialize animation data: " + std::string(e.what()))
                return false;
            }
            std::ofstream outFile(savePath);
            if (outFile.is_open())
            {
                outFile << jsonStr;
                outFile.close();
                if (!outFile)
                {
                    SS_DEBUG_LOG("[ANIMATION TOOL] Failed to write animation data to: " + savePath.string())
                    return false;
                }
            } else {
                SS_DEBUG_LOG("[ANIMATION TOOL] Failed to save animation data to: " + savePath.string())
                return false;
            }
            SS_DEBUG_LOG("[ANIMATION TOOL] Successfully saved animation data to: " + savePath.string())
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            SS_DEBUG_LOG("[ANIMATION TOOL] Failed to create animations directory: " + std::string(e.what()))
            return false;
        }
        return true;
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
        if (ImGui::Button("Select Animation JSON"))
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
        static bool showError = false;
        const char* errorMessage = "Something went wrong - check log";

        CenterNext();
        if (ImGui::BeginPopupModal(selectorPopup))
        {
            if (showError)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorMessage);
            }
            static AssetNode* selectedNode = nullptr;
            DrawTextureBrowser(selectedNode);
            if (ImGui::InputText("Animation ID", newAnimDatabuf, IM_ARRAYSIZE(newAnimDatabuf), ImGuiInputTextFlags_CharsNoBlank)) {}
            bool bufferValid = newAnimDatabuf[0] != '\0';
            ImGui::BeginDisabled(!bufferValid || !selectedNode);
            if (ImGui::Button("Confirm"))
            {
                std::string selectedID = selectedNode->m_Name;
                AnimationData data;
                data.ID = newAnimDatabuf;
                data.textureID = selectedID;
                
                // save animation data to the animations directory
                if (SaveAnimationData(data, m_AnimationsDir))
                {
                    if (!ResourceManager::Get().GetTextures().count(selectedID))
                    {
                        ResourceManager::Get().LoadResource<sf::Texture>(selectedNode->m_Name, selectedNode->m_Path);
                    }

                    if (ResourceManager::Get().LoadAnimationData(data.ID, data))
                    {    
                        Open(ResourceManager::Get().GetAnimationData(data.ID));

                        // clear input buffer and selection
                        newAnimDatabuf[0] = '\0';
                        selectedNode = nullptr;
                        showCreate = false;
                        editTool = true;
                        showError = false;
                        ImGui::CloseCurrentPopup();
                    }
                    else showError = true;
                } 
                else showError = true;
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                // clear input buffer and selection
                newAnimDatabuf[0] = '\0';
                selectedNode = nullptr;
                showCreate = false;
                showError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void AnimationTool::DrawLoadModal()
    {
        if (!showLoad)
            return;

        m_AnimationsDir = Application::Get().GetProjectManager().GetCurrentProjectPaths().Animations();

        if (!ImGui::IsPopupOpen("Open Animation Data"))
            ImGui::OpenPopup("Open Animation Data");

        static bool showError = false;
        const char* errorMessage = "Failed to load animation data. Check logs";

        CenterNext();
        if (ImGui::BeginPopupModal("Open Animation Data", &showLoad))
        {
            if (ImGui::IsWindowAppearing())
                showError = false;
            if (showError)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorMessage);
            }

            try {
                if (std::filesystem::create_directories(m_AnimationsDir))
                {
                    SS_DEBUG_LOG("[ANIMATION TOOL] Created new animations directory...")
                }

                bool empty = true;
                for (const auto& entry : std::filesystem::directory_iterator(m_AnimationsDir))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".json")
                    {
                        empty = false;
                        std::string fileName = entry.path().stem().string();
                        if (ImGui::Selectable(fileName.c_str()))
                        {
                            // load the selected animation data and open it in the editor
                            if (ResourceManager::Get().LoadAnimationFromPath(entry.path()))
                            {
                                Open(ResourceManager::Get().GetAnimationData(fileName));
                                showLoad = false;
                                showError = false;
                                ImGui::CloseCurrentPopup();
                            } else showError = true;
                        }
                    }
                }
                if (empty)
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", "No animation files found.");
                }
            } catch (const std::filesystem::filesystem_error& e)
            {
                SS_DEBUG_LOG("[ANIMATION TOOL] File system error during animation file listing: " + std::string(e.what()))
                showError = true;
            }
            if (ImGui::Button("Cancel"))
            {
                showLoad = false;
                showError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void AnimationTool::DrawImportModal()
    {
        static bool showImportError = false;
        const char* importErrorMsg = "Failed to import animation JSON. Check logs";

        if (!showImport)
            return;
        if (!ImGui::IsPopupOpen("Import Animation Data"))
            ImGui::OpenPopup("Import Animation Data");

        CenterNext();
        if (ImGui::BeginPopupModal("Import Animation Data"))
        {
            if (ImGui::IsWindowAppearing())
                showImportError = false;
            if (showImportError)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), importErrorMsg);
                if (ImGui::Button("Dismiss"))
                    showImportError = false;
            }

            // select texture
            static AssetNode* selectedNode = nullptr;
            DrawTextureBrowser(selectedNode);

            // select animation JSON file
            static std::filesystem::path selectedJsonPath;
            DrawAnimationFileBrowser(selectedJsonPath);

            ImGui::BeginDisabled(!selectedNode || selectedJsonPath.empty());
            if (ImGui::Button("Import"))
            {
                showImportError = false;
                try {
                    ImportJSON(selectedNode, selectedJsonPath);
                } catch (...) {
                    showImportError = true;
                }
                if (!showImportError)
                {
                    showImport = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndDisabled();
            if (ImGui::Button("Cancel")) 
            {
                showImport = false;
                showImportError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}