#include "ProjectManager.h"
#include "Core/Application.h"
#include "Core/Serialization/Serializer.h"
#include "Editor/Editor.h"
#include "Editor/Utils/EditorSettings.h"
#include "Editor/Blueprints/Blueprint.h"

#include "nlohmann/json.hpp"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

using json = nlohmann::json;

namespace Spoon
{
    Project* ProjectManager::CreateNew(std::string newProjectBuf)
    {
        // create new project structure
        Project newProject;
        newProject.ID = newProjectBuf;
        newProject.blueprints = GetDefaultBlueprints();

        // Create dirs
        std::filesystem::create_directories("projects/");
        newProject.filePath = std::filesystem::path("projects/") / (newProject.ID + ".json");

        // Open the project file for writing
        std::ofstream newProj(newProject.filePath);
        if (!newProj.is_open())
        {
            return nullptr;
        }

        newProject.dataPath = std::filesystem::path("projects/") / newProject.ID / "data/";
        std::filesystem::create_directories(newProject.dataPath);
        newProject.assetsPath = std::filesystem::path("projects/") / newProject.ID / "assets/";
        std::filesystem::create_directories(newProject.assetsPath);
        newProject.presetsPath = newProject.assetsPath / "presets/";
        std::filesystem::create_directories(newProject.presetsPath);
        newProject.version = "1.0";

        // Dump data to JSON
        json projectJson;
        projectJson["ID"] = newProject.ID;
        projectJson["DataPath"] = newProject.dataPath;
        projectJson["AssetsPath"] = newProject.assetsPath;
        projectJson["PresetsPath"] = newProject.presetsPath;
        projectJson["Version"] = newProject.version;
        projectJson["RecentFiles"] = newProject.recentFiles;
        projectJson["Config"] = ProjectConfig{};
        projectJson["Blueprints"] = newProject.blueprints; 
        newProj << projectJson.dump(4);
        newProj.close();

        return &newProject;
    }

    void ProjectManager::LoadProject(Editor* editor)
    {
            

        if (m_CurrentProject)
        {
            editor->SetCurrentProject(m_CurrentProject.get());
            editor->OpenProject = false;
        }
    }

    void ProjectManager::ConfigProject(Editor* editor)
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Project Settings", &editor->ProjectProperties))
        {
            char projectName[128];
            char version[32];
            char dataPath[512];
            char assetsPath[512];
            
            std::snprintf(projectName, sizeof(projectName), "%s", m_CurrentProject->ID.c_str());
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
                 m_CurrentProject->ID = projectName;
                 m_CurrentProject->version = version;
                 m_CurrentProject->dataPath = dataPath;
                 m_CurrentProject->assetsPath = assetsPath;
            }

            ImGui::TextDisabled("Settings");
            ImGui::Separator();

            changed |= ImGui::Checkbox("Physics Enabled", &m_CurrentProject->config.PhysicsEnabled);
            ImGui::SameLine(); HelpMarker("TODO: not implemented");

            static constexpr ActiveSortPolicy policies[] = {
                ActiveSortPolicy::Isometric,
                ActiveSortPolicy::TopDown
            };
            ActiveSortPolicy* selectedPolicy = nullptr;
            if (ImGui::BeginListBox("Projection Style"))
            {
                for (const ActiveSortPolicy policy : policies)
                {
                    const bool selected = m_CurrentProject->config.SortPolicy == policy;

                    if (ImGui::Selectable(SortPolicyToString(policy), selected))
                        m_CurrentProject->config.SortPolicy = policy;

                    if (selected)
					{
						Application::Get().GetRenderer().UpdateRenderConfig();
                        ImGui::SetItemDefaultFocus();
					}
                }
                ImGui::EndListBox();
            }
            ImGui::SameLine(); HelpMarker("Determines the rendering style.");
            
            ImGui::Separator();
            if (ImGui::Button("Save Project"))
               editor->SaveProject = true;
            
               ImGui::SameLine();
            if (ImGui::Button("Close"))
                editor->ProjectProperties = false;
        }
        ImGui::End();
    }

    Project* ProjectManager::OpenFromFile(const std::filesystem::path& filepath)
    {
        std::ifstream fileStream(filepath);
        if (!fileStream.is_open())
        {

            return nullptr;
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
            newProject.presetsPath = projectJson.value("PresetsPath", newProject.assetsPath / "presets/");
            newProject.version = projectJson.value("Version", "1.0");
            newProject.recentFiles = projectJson.value("RecentFiles", std::vector<std::filesystem::path>{});
			newProject.config = projectJson.value("Config", ProjectConfig{});
            newProject.blueprints = projectJson.value("Blueprints", GetDefaultBlueprints());

            Application::Get().GetSceneManager().LoadManifest(newProject.dataPath.string());
            ResourceManager::Get().ScanAssets(newProject.assetsPath);
            m_CurrentProject = std::make_unique<Project>(newProject);
        }
        catch (const json::exception& e)
        {
            // Handle JSON parsing error
            fileStream.close();
            return nullptr;
        }
        return m_CurrentProject.get();
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
            json projectJson;
            projectJson["ID"] = m_CurrentProject->ID;
            projectJson["DataPath"] = m_CurrentProject->dataPath.string();
            projectJson["AssetsPath"] = m_CurrentProject->assetsPath.string();
            projectJson["PresetsPath"] = m_CurrentProject->presetsPath.string();
            projectJson["Version"] = m_CurrentProject->version;
			projectJson["Config"] = m_CurrentProject->config;
            projectJson["Blueprints"] = m_CurrentProject->blueprints;

            const std::string serializedProject = projectJson.dump(4);
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

            fileStream << serializedProject;
            fileStream.close();

            // Reset flags
            m_Saving = false;
            editor->SaveProject = false;
        }
    }

    std::vector<Blueprint>& ProjectManager::GetBlueprints()
    {
        if (!m_CurrentProject)
            throw std::runtime_error("fetching blueprints with no project loaded");

        return m_CurrentProject->blueprints;
    }

    // get all presets from the assets folder by category
    // ex: GetAvailablePresets("particles") will return all JSON presets in the "particles" category folder.
    std::vector<std::string> ProjectManager::GetAvailablePresets(const std::string& category)
    {
        if (!m_CurrentProject)
            throw std::runtime_error("fetching available presets with no project loaded");

        // verify category is a valid dir
        if (!std::filesystem::exists(m_CurrentProject->presetsPath / category) || !std::filesystem::is_directory(m_CurrentProject->presetsPath / category))
            throw std::runtime_error("invalid presets category: " + category);

        std::vector<std::string> presets;
        for (const auto& entry : std::filesystem::directory_iterator(m_CurrentProject->presetsPath / category))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                presets.push_back(entry.path().stem().string());
            }
        }
        return presets;
    }
}
