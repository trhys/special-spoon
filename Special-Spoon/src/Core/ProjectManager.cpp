#include "ProjectManager.h"
#include "Core/Application.h"
#include "Serialization/Serializer.h"
#include "Editor/Editor.h"
#include "Editor/Utils/EditorSettings.h"

#include "nlohmann/json.hpp"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

using json = nlohmann::json;

namespace Spoon
{
    void ProjectManager::CreateNew(Editor* editor)
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Create New Project", &editor->NewProject, ImGuiWindowFlags_AlwaysAutoResize);

        static char newProjectBuf[64] = "";
        ImGui::InputText("Project Name", newProjectBuf, IM_ARRAYSIZE(newProjectBuf));
        if (ImGui::Button("Confirm"))
        {
            if (strlen(newProjectBuf) == 0)
                return;
            Project newProject;
            newProject.ID = std::string(newProjectBuf);
            newProject.filePath = std::filesystem::path("projects/") / (newProject.ID + ".json");
            newProject.dataPath = std::filesystem::path("projects/") / newProject.ID / "data/";
            newProject.assetsPath = std::filesystem::path("projects/") / newProject.ID / "assets/";
            newProject.version = "1.0";

            ResourceManager::Get().ScanAssets(newProject.assetsPath);
            Application::Get().GetSceneManager().LoadManifest(newProject.dataPath.string());
            m_CurrentProject = std::make_unique<Project>(newProject);
            editor->SetCurrentProject(m_CurrentProject.get());
            editor->NewProject = false;
        }
        ImGui::End();
    }

    void ProjectManager::LoadProject(Editor* editor)
    {
        // Open file dialog to select a project file
        ImGuiFileDialog::Instance()->OpenDialog("LoadProject", "Choose Project File", ".json");        
        if (ImGuiFileDialog::Instance()->Display("LoadProject"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::filesystem::path pathObj(filePathName);

                // Load the project using the selected file path
                OpenFromFile(pathObj);
            }
            
            // Close the dialog
            ImGuiFileDialog::Instance()->Close();
            if (m_CurrentProject)
            {
                editor->SetCurrentProject(m_CurrentProject.get());
                editor->OpenProject = false;
            }
        }
    }

    void ProjectManager::OpenFromFile(const std::filesystem::path& filepath)
    {
        std::ifstream fileStream(filepath);
        if (!fileStream.is_open())
        {
            // Handle error: unable to open file
            ImGui::OpenPopup("Error Opening Project File");
            if (ImGui::BeginPopupModal("Error Opening Project File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Failed to open project file at path: %s", filepath.string().c_str());
                if (ImGui::Button("OK"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            return;
        }

        try {
            // Deserialize project data from the file
            json projectJson;
            projectJson = json::parse(fileStream);
            fileStream.close();

            Project newProject;
            newProject.ID = projectJson.value("ID", "UnknownProject");
            newProject.filePath = filepath;
            newProject.dataPath = projectJson.value("DataPath", "");
            newProject.assetsPath = projectJson.value("AssetsPath", "");
            newProject.version = projectJson.value("Version", "1.0");

            Application::Get().GetSceneManager().LoadManifest(newProject.dataPath.string());
            ResourceManager::Get().ScanAssets(newProject.assetsPath);
            m_CurrentProject = std::make_unique<Project>(newProject);
        }
        catch (const json::exception& e)
        {
            // Handle JSON parsing error
            fileStream.close();
            return;
        }
    }

    void ProjectManager::SaveProject(Editor* editor)
    {
        if (!m_CurrentProject)
        {
            // No project loaded to save
            return;
        }

        if (!ImGui::IsPopupOpen("Prompt Save Project") && !EditorSettings::Get().skipProjectSavePrompt)
            ImGui::OpenPopup("Prompt Save Project");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        // Prompt to save project
        if (ImGui::BeginPopupModal("Prompt Save Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        { 
            ImGui::Text("Do you want to save the current project?\n This will overwrite all existing files.");
            ImGui::Separator();
            if (ImGui::Button("Yes"))
            {
                m_Saving = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No"))
            {
                editor->SaveProject = false;
                ImGui::CloseCurrentPopup();
                return;
            }
            ImGui::Checkbox("Don't ask me again", &EditorSettings::Get().skipProjectSavePrompt);
            ImGui::EndPopup();
        }

        if (m_Saving)
        {
            std::ofstream fileStream(m_CurrentProject->filePath, std::ios::out | std::ios::trunc);
            if (!fileStream.is_open())
            {
                // Handle error: unable to open file for writing
                ImGui::OpenPopup("Error Saving Project File");
                if (ImGui::BeginPopupModal("Error Saving Project File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Failed to save project file at path: %s", m_CurrentProject->filePath.string().c_str());
                    if (ImGui::Button("OK"))
                    {
                        m_Saving = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                return;
            }

            // Dump data to JSON
            json projectJson;
            projectJson["ID"] = m_CurrentProject->ID;
            projectJson["DataPath"] = m_CurrentProject->dataPath.string();
            projectJson["AssetsPath"] = m_CurrentProject->assetsPath.string();
            projectJson["Version"] = m_CurrentProject->version;

            fileStream << projectJson.dump(4);
            fileStream.close();

            // Reset flags
            m_Saving = false;
            editor->SaveProject = false;
        }
        
    }
}