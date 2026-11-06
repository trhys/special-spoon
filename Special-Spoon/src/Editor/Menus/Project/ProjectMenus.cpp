#include "Editor/Editor.h"
#include "Editor/Blueprints/ProjectBlueprint.h"
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
                OpenFileDialog(DialogOption::Open);
                m_ShowNoProjectLoaded = false;
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
            static int selectedTemplate = -1; // -1 = empty project
            if (ImGui::InputText("Project Name", newProjectBuf, IM_ARRAYSIZE(newProjectBuf))) {}

            // built-in project blueprints to start from
            const auto& templates = GetBuiltInProjectTemplates();
            ImGui::SeparatorText("Template");
            if (ImGui::BeginListBox("##ProjectTemplates", ImVec2(320, ImGui::GetTextLineHeightWithSpacing() * 5)))
            {
                if (ImGui::Selectable("Empty Project", selectedTemplate == -1))
                    selectedTemplate = -1;

                for (int i = 0; i < static_cast<int>(templates.size()); i++)
                {
                    const auto& blueprint = templates[i].m_Blueprint;
                    const std::string label = blueprint.m_Name + " (" + blueprint.m_Profile + ")";
                    if (ImGui::Selectable(label.c_str(), selectedTemplate == i))
                        selectedTemplate = i;
                }
                ImGui::EndListBox();
            }
            if (ImGui::Button("Confirm"))
            {
                if (strlen(newProjectBuf) == 0)
                {
                    createNewError = true; // set error flag
                    ImGui::End();
                    return;
                }

                const ProjectBlueprint* projectBlueprint =
                    (selectedTemplate >= 0 && selectedTemplate < static_cast<int>(templates.size()))
                    ? &templates[selectedTemplate].m_Blueprint : nullptr;

                Project* newProject = m_Services.projectManager->CreateNew(std::string(newProjectBuf));
                if (newProject && projectBlueprint)
                {
                    // compose the project config before the editor applies it:
                    // explicit overrides > profile defaults > engine fallback
                    try {
                        newProject->config = ComposeProjectConfig(
                            projectBlueprint->m_Profile, projectBlueprint->m_ConfigOverrides);
                    }
                    catch (const json::exception& e)
                    {
                        SS_DEBUG_LOG("[PROJECT] Invalid config overrides in template '"
                                     + projectBlueprint->m_Name + "': " + std::string(e.what()))
                    }
                    m_Services.projectManager->SaveProject(newProject);
                }

                if (!SetCurrentProject(newProject))
                {
                    m_ShowCreateProjectError = true;
                    ImGui::End();
                    return; // return early
                }

                // scaffold and load the template's starter scene
                if (projectBlueprint && !projectBlueprint->m_Scene.empty())
                {
                    try
                    {
                        SceneData* scene = ScaffoldStarterScene(
                            *projectBlueprint, 
                            *m_Services.sceneManager, 
                            newProject->paths,
                            newProject->config
                        );
                        m_Services.sceneManager->LoadScene(
                            scene->ID, *m_Services.entityManager, *m_Services.systemManager);
                        SetActiveScene(scene);
                    }
                    catch (const std::exception& e)
                    {
                        SS_DEBUG_LOG("[PROJECT] Failed to scaffold starter scene: " + std::string(e.what()))
                    }
                }

                createNewError = false;     // clear the error flag
                selectedTemplate = -1;      // reset for the next prompt
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

    void Editor::OpenFileDialog(DialogOption option)
    {
        switch (option) {
            case DialogOption::Open :
                ImGuiFileDialog::Instance()->OpenDialog(
                    "LoadProject", "Choose Project File", ".sproj");
                m_ShowLoadProject = true;
                break;
        }
    }

    void Editor::DrawLoadProject()
    {
        if (!m_ShowLoadProject)
            return;

        auto* dialog = ImGuiFileDialog::Instance();
        if (!dialog->Display("LoadProject"))
            return;

        if (dialog->IsOk())
        {
            std::string filePathName = dialog->GetFilePathName();
            std::filesystem::path pathObj(filePathName);

            // Load the project using the selected file path
            bool loaded = SetCurrentProject(m_Services.projectManager->OpenFromFile(pathObj));
            if (!loaded)
                m_ShowLoadProjectError = true; // set error flag
        }
        dialog->Close();
        m_ShowLoadProject = false;
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

            std::snprintf(projectName, sizeof(projectName), "%s", m_CurrentProject->name.c_str());
            std::snprintf(version, sizeof(version), "%s", m_CurrentProject->version.c_str());

            const ProjectPaths& paths = m_CurrentProject->paths;
            ImGui::TextDisabled("Project file");
            ImGui::TextWrapped("%s", (paths.m_ProjectRoot / (m_CurrentProject->name + ".sproj")).string().c_str());
            ImGui::TextDisabled("Data directory");
            ImGui::TextWrapped("%s", paths.Data().string().c_str());
            ImGui::TextDisabled("Assets directory");
            ImGui::TextWrapped("%s", paths.Assets().string().c_str());
            ImGui::Separator();
        
			bool changed = false;
            changed |= ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));
            changed |= ImGui::InputText("Version", version, IM_ARRAYSIZE(version));

            if (changed)
            {
                m_CurrentProject->name = projectName;
                m_CurrentProject->version = version;
			}

            ImGui::TextDisabled("Settings");
            ImGui::Separator();

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

        if (!m_SkipOverwriteSavePrompt)
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
                }
                ImGui::Checkbox("Don't ask me again", &m_SkipOverwriteSavePrompt);
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
            ImGui::Text("Failed to save project file at path: %s",
                (m_CurrentProject->paths.m_ProjectRoot / (m_CurrentProject->name + ".sproj")).string().c_str());
            if (ImGui::Button("OK"))
            {
				m_ShowSaveProjectError = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
	}
}