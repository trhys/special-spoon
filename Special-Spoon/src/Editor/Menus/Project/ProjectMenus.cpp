#include "ProjectMenus.h"
#include "Editor/Editor.h"
#include "Editor/Utils/Windows.h"
#include "Editor/Utils/Tooltips.h"

namespace Spoon
{
    void Editor::NoProjectLoaded()
    {
        if (!ImGui::IsPopupOpen("No Project Loaded"))
            ImGui::OpenPopup("No Project Loaded");

        CenterNext();
        if (ImGui::BeginPopupModal("No Project Loaded", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("No project is currently loaded. Please create or open a project to continue.");
            if (ImGui::Button("Create"))
            {
                m_ShowCreateProjectPrompt = true;
                m_ShowNoProjectLoaded = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Open"))
            {
                m_ShowNoProjectLoaded = false;
                m_LoadProject = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void Editor::DrawCreateProjectPrompt()
    {
        static bool createNewError = false; // error flag
        if (m_ShowCreateProjectError)
            DrawCreateProjectError();

        CenterNext(); // center the window
        if (ImGui::Begin("Create New Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (createNewError) ErrorMessage("Project name cannot be empty.");

            static char newProjectBuf[64] = "";
            if (ImGui::InputText("Project Name", newProjectBuf, IM_ARRAYSIZE(newProjectBuf))) {}
            if (ImGui::Button("Confirm"))
            {
                if (strlen(newProjectBuf) == 0)
                {
                    createNewError = true; // set error flag
                    ImGui::End();
                    return;
                }
                    
                Project* newProject = m_Services.projectManager->CreateProject(newProjectBuf);
                if (!SetCurrentProject(newProject))
                {
                    m_ShowCreateProjectError = true;
                    ImGui::End();
                    return; // return early
                }

                createNewError = false;     // clear the error flag
                m_ShowCreateProjectPrompt = false;    // reset the create project flag
            }
        }
        ImGui::End();
    }

    void Editor::DrawCreateProjectError()
    {
        if (!ImGui::IsPopupOpen("Error Creating Project File"))
            ImGui::OpenPopup("Error Creating Project File");

        if (ImGui::BeginPopupModal("Error Creating Project File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Failed to create project file. Check log for path.");
            if (ImGui::Button("OK"))
            {
                m_ShowCreateProjectError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    bool Editor::SetCurrentProject(Project* project)
    {
        if (!project)
            return false;
        SetActiveScene(nullptr);
        ResourceManager::Get().ScanAssets(project->assetsPath);
        m_Services.sceneManager->LoadManifest(project->dataPath);
        m_Services.renderer->UpdateRenderConfig();
        m_CurrentProject = project;
        m_ShowCreateProjectPrompt = false;
        return true;
    }

    void Editor::OpenFileDialog()
    {
        ImGuiFileDialog::Instance()->OpenDialog(
            "LoadProject", "Choose Project File", ".json");
    }

    void Editor::DrawLoadProject()
    {
        auto* dialog = ImGuiFileDialog::Instance();
        if (dialog->Display("LoadProject"))
        {
            if (dialog->IsOk())
            {
                std::string filePathName = dialog->GetFilePathName();
                std::filesystem::path pathObj(filePathName);

                // Load the project using the selected file path
                //SetActiveScene(nullptr);
                project = m_Services.projectManager->OpenFromFile(pathObj);
                if (!project)
                {
                    m_ShowLoadProjectError = true; // set error flag
                }
				Application::Get().GetRenderer().UpdateRenderConfig();
            }
            // Close the dialog
            dialog->Close();
        }
    }
}