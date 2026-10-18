#include "Editor/Editor.h"
#include "Editor/Utils/EditorSettings.h"
#include "Editor/Utils/Windows.h"
#include "Editor/Utils/Tooltips.h"

#include "Core/Project/ProjectManager.h"
#include "Core/Renderer/Renderer.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace Spoon
{
    void Editor::NoProjectLoaded()
    {
        if (!m_ShowNoProjectLoaded)
			return;

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
                OpenFileDialog();
                m_ShowNoProjectLoaded = false;
                m_ShowLoadProject = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void Editor::DrawCreateProjectPrompt()
    {
        if (!m_ShowCreateProjectPrompt)
			return;

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
                    
                Project* newProject = m_Services.projectManager->CreateNew(std::string(newProjectBuf));
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
        if (!m_ShowCreateProjectError)
	    	return;

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

    void Editor::OpenFileDialog()
    {
        ImGuiFileDialog::Instance()->OpenDialog(
            "LoadProject", "Choose Project File", ".json");
		m_ShowLoadProject = true;
    }

    void Editor::DrawLoadProject()
    {
        if (!m_ShowLoadProject)
            return;

        auto* dialog = ImGuiFileDialog::Instance();
        if (dialog->Display("LoadProject"))
        {
            if (dialog->IsOk())
            {
                std::string filePathName = dialog->GetFilePathName();
                std::filesystem::path pathObj(filePathName);

                // Load the project using the selected file path
                Project* project = m_Services.projectManager->OpenFromFile(pathObj);
                if (!project)
                {
                    m_ShowLoadProjectError = true; // set error flag
                }
                else {
                    SetCurrentProject(project);
                }
            }            
        }
        dialog->Close();
        
        if (m_CurrentProject)
			m_ShowLoadProject = false; // close the load project dialog if a project is loaded
    }

    void Editor::DrawLoadProjectError()
    {
        if (!m_ShowLoadProjectError)
			return;

        if (!ImGui::IsPopupOpen("Error Loading Project File"))
            ImGui::OpenPopup("Error Loading Project File");
        if (ImGui::BeginPopupModal("Error Loading Project File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Failed to load project file. Check log for path.");
            if (ImGui::Button("OK"))
            {
                m_ShowLoadProjectError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
	}

    void Editor::DrawConfigProject()
    {
        if (!m_ShowConfigProject)
            return;

		CenterNext();
        if (ImGui::Begin("Project Settings", &m_ShowConfigProject))
        {
            char projectName[128];
            char version[32];
            char dataPath[512];
            char assetsPath[512];

            std::snprintf(projectName, sizeof(projectName), "%s", m_CurrentProject->name.c_str());
            std::snprintf(version, sizeof(version), "%s", m_CurrentProject->version.c_str());
            std::snprintf(dataPath, sizeof(dataPath), "%s", m_CurrentProject->dataPath.string().c_str());
            std::snprintf(assetsPath, sizeof(assetsPath), "%s", m_CurrentProject->assetsPath.string().c_str());

            ImGui::TextDisabled("Project file");
            ImGui::TextWrapped("%s", m_CurrentProject->filePath.string().c_str());
            ImGui::Separator();
        
			bool changed = false;
            changed |= ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));
            changed |= ImGui::InputText("Version", version, IM_ARRAYSIZE(version));
            changed |= ImGui::InputText("Data Path", dataPath, IM_ARRAYSIZE(dataPath));
            changed |= ImGui::InputText("Assets Path", assetsPath, IM_ARRAYSIZE(assetsPath));

            if (changed)
            {
                m_CurrentProject->name = projectName;
                m_CurrentProject->version = version;
                m_CurrentProject->dataPath = dataPath;
                m_CurrentProject->assetsPath = assetsPath;
			}

            ImGui::TextDisabled("Settings");
            ImGui::Separator();

            ImGui::Checkbox("Physics Enabled", &m_CurrentProject->config.PhysicsEnabled);
            ImGui::SameLine(); HelpMarker("TODO: not implemented");

            static constexpr ActiveSortPolicy policies[] = {
                ActiveSortPolicy::Isometric,
                ActiveSortPolicy::TopDown
            };
			ActiveSortPolicy selectedPolicy = m_CurrentProject->config.SortPolicy;
            if (ImGui::BeginListBox("Projection Style"))
            {
                for (const ActiveSortPolicy policy : policies)
                {
                    const bool selected = selectedPolicy == policy;

                    if (ImGui::Selectable(SortPolicyToString(policy), selected))
						m_CurrentProject->config.SortPolicy = policy;

                    if (selected)
                    {
                        m_Services.renderer->UpdateRenderConfig();
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::SameLine(); HelpMarker("Determines the rendering style.");

            ImGui::Separator();
            if (ImGui::Button("Save Project"))
            {
				m_ShowOverwriteProjectPrompt = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Close"))
				m_ShowConfigProject = false;
        }
        ImGui::End();
    }

    void Editor::DrawOverwriteProjectPrompt()
    {
        if (!m_ShowOverwriteProjectPrompt)
			return;

        if (!EditorSettings::Get().skipProjectSavePrompt)
        {
            if (!ImGui::IsPopupOpen("Prompt Save Project"))
                ImGui::OpenPopup("Prompt Save Project");

            CenterNext();

            if (ImGui::BeginPopupModal("Prompt Save Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Do you want to save the current project?\n This will overwrite all existing files.");
                ImGui::Separator();
                if (ImGui::Button("Yes"))
                {
                    Project* projectPtr = m_Services.projectManager->SaveProject(m_CurrentProject);
                    if (!projectPtr)
                    {
                        m_ShowSaveProjectError = true;
                    }
                    m_ShowOverwriteProjectPrompt = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No"))
                {
					m_ShowOverwriteProjectPrompt = false;
                    ImGui::CloseCurrentPopup();
                    return;
                }
                ImGui::Checkbox("Don't ask me again", &EditorSettings::Get().skipProjectSavePrompt);
                ImGui::EndPopup();
            }
        }
        else
        {
            Project* projectPtr = m_Services.projectManager->SaveProject(m_CurrentProject);
            if (!projectPtr)
                m_ShowSaveProjectError = true;
			m_ShowOverwriteProjectPrompt = false;
        }
    }

    void Editor::DrawSaveProjectError()
    {
        if (!m_ShowSaveProjectError)
            return;

        // Handle error: unable to open file for writing
        ImGui::OpenPopup("Error Saving Project File");
        if (ImGui::BeginPopupModal("Error Saving Project File", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Failed to save project file at path: %s", m_CurrentProject->filePath.string().c_str());
            if (ImGui::Button("OK"))
            {
				m_ShowSaveProjectError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
	}
}