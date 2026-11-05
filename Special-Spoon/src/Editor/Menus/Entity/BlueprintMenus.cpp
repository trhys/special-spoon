#include "Editor/Editor.h"
#include "Editor/Blueprints/Blueprint.h"
#include "Editor/Utils/Windows.h"
#include "Editor/Utils/Helpmarker.h"

#include "Imgui/misc/cpp/imgui_stdlib.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>

namespace Spoon
{
    namespace
    {
        // recursively draw a directory as a tree: subdirectories as folders,
        // .json blueprint files as selectable leaves
        void DrawBlueprintTree(const std::filesystem::path& dir, const std::filesystem::path& selected,
                               std::filesystem::path& clicked)
        {
            std::vector<std::filesystem::directory_entry> subDirs;
            std::vector<std::filesystem::directory_entry> files;
            for (const auto& entry : std::filesystem::directory_iterator(dir))
            {
                if (entry.is_directory())
                    subDirs.push_back(entry);
                else if (entry.is_regular_file() && entry.path().extension() == ".json")
                    files.push_back(entry);
            }

            const auto byName = [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b)
            {
                return a.path().filename() < b.path().filename();
            };
            std::sort(subDirs.begin(), subDirs.end(), byName);
            std::sort(files.begin(), files.end(), byName);

            for (const auto& subDir : subDirs)
            {
                // use the full path as the ID to avoid collisions with same-named files
                if (ImGui::TreeNodeEx(subDir.path().string().c_str(), ImGuiTreeNodeFlags_SpanAvailWidth,
                                      "%s", subDir.path().filename().string().c_str()))
                {
                    DrawBlueprintTree(subDir.path(), selected, clicked);
                    ImGui::TreePop();
                }
            }
            for (const auto& file : files)
            {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen
                    | ImGuiTreeNodeFlags_SpanAvailWidth;
                if (file.path() == selected)
                    flags |= ImGuiTreeNodeFlags_Selected;
                ImGui::TreeNodeEx(file.path().string().c_str(), flags,
                                  "%s", file.path().filename().string().c_str());
                if (ImGui::IsItemClicked())
                    clicked = file.path();
            }
        }

        // editable state for the currently selected blueprint
        struct ComponentEdit
        {
            std::string m_Name;
            std::string m_DataText; // component data as editable JSON text
        };

        struct BlueprintEditState
        {
            std::filesystem::path m_Path;
            std::string m_Name;
            std::vector<ComponentEdit> m_Components;
            bool m_Loaded = false;
            bool m_Dirty = false;
            std::string m_Status;
        };

        bool LoadBlueprintEdit(const std::filesystem::path path, BlueprintEditState& state)
        {
            state = BlueprintEditState{};
            state.m_Path = path;

            std::ifstream file(path);
            if (!file.is_open())
            {
                SS_DEBUG_LOG("[BLUEPRINTS] Failed to open blueprint file: " + path.string())
                state.m_Status = "Failed to open blueprint file. Check logs";
                return false;
            }

            try
            {
                json blueprintJson;
                file >> blueprintJson;
                EntityBlueprint blueprint = blueprintJson.get<EntityBlueprint>();

                state.m_Name = blueprint.m_Name;
                for (const auto& comp : blueprint.m_Components)
                    state.m_Components.push_back({ comp.m_Name, comp.m_Data.dump(4) });
                state.m_Loaded = true;
                return true;
            }
            catch (const json::exception& e)
            {
                SS_DEBUG_LOG("[BLUEPRINTS] Failed to parse blueprint JSON: " + std::string(e.what()))
                state.m_Status = "Failed to parse blueprint JSON. Check logs";
                return false;
            }
        }

        bool SaveBlueprintEdit(BlueprintEditState& state)
        {
            EntityBlueprint blueprint;
            blueprint.m_Name = state.m_Name;
            for (const auto& comp : state.m_Components)
            {
                try
                {
                    blueprint.m_Components.push_back({ comp.m_Name, json::parse(comp.m_DataText) });
                }
                catch (const json::exception& e)
                {
                    state.m_Status = "Invalid JSON in '" + comp.m_Name + "': " + e.what();
                    return false;
                }
            }

            std::ofstream file(state.m_Path, std::ios::out | std::ios::trunc);
            if (!file.is_open())
            {
                SS_DEBUG_LOG("[BLUEPRINTS] Failed to write blueprint file: " + state.m_Path.string())
                state.m_Status = "Failed to write blueprint file. Check logs";
                return false;
            }

            const json blueprintJson = blueprint;
            file << blueprintJson.dump(4);
            state.m_Dirty = false;
            state.m_Status = "Saved";
            return true;
        }

        // serialize a default-constructed component of the given type by
        // briefly running its creator on a throwaway probe entity
        json DefaultComponentData(EntityManager& manager, const std::string& type)
        {
            json data = json::object();
            const auto& creators = manager.GetCreators();
            const auto found = creators.find(type);
            if (found == creators.end())
                return data;

            const UUID probe = manager.CreateEntity("blueprint-defaults-probe");
            try
            {
                found->second(probe);
                for (Component* comp : manager.GetAllComponentsOfEntity(probe))
                {
                    if (comp->GetDisplayName() == type)
                        data = comp->Serialize();
                }
            }
            catch (const std::exception& e)
            {
                SS_DEBUG_LOG("[BLUEPRINTS] Failed to generate defaults for '" + type + "': " + std::string(e.what()))
                data = json::object();
            }
            // always reap the probe so it never leaks into the live scene
            manager.KillEntity(probe);
            return data;
        }

        void DrawBlueprintEditor(BlueprintEditState& state, EntityManager& manager)
        {
            if (!state.m_Loaded)
            {
                if (!state.m_Status.empty())
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", state.m_Status.c_str());
                return;
            }

            if (ImGui::InputText("Name", &state.m_Name))
                state.m_Dirty = true;
            ImGui::SameLine(); HelpMarker("Display name of the blueprint. Renaming here does not rename the file on disk");
            ImGui::Separator();

            int removeIndex = -1;
            for (int i = 0; i < static_cast<int>(state.m_Components.size()); i++)
            {
                auto& comp = state.m_Components[i];
                ImGui::PushID(i);
                if (ImGui::CollapsingHeader(comp.m_Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::InputTextMultiline("##data", &comp.m_DataText,
                                                  ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8)))
                        state.m_Dirty = true;
                    if (ImGui::SmallButton("Remove Component"))
                        removeIndex = i;
                }
                ImGui::PopID();
            }
            if (removeIndex >= 0)
            {
                state.m_Components.erase(state.m_Components.begin() + removeIndex);
                state.m_Dirty = true;
            }

            // add a component seeded with its engine defaults
            if (ImGui::BeginCombo("##AddComponent", "Add Component..."))
            {
                std::vector<std::string> types;
                for (const auto& [type, creator] : manager.GetCreators())
                    types.push_back(type);
                std::sort(types.begin(), types.end());

                for (const auto& type : types)
                {
                    const bool alreadyHas = std::any_of(state.m_Components.begin(), state.m_Components.end(),
                        [&type](const ComponentEdit& comp) { return comp.m_Name == type; });
                    if (alreadyHas)
                        continue;

                    if (ImGui::Selectable(type.c_str()))
                    {
                        state.m_Components.push_back({ type, DefaultComponentData(manager, type).dump(4) });
                        state.m_Dirty = true;
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();
            ImGui::BeginDisabled(!state.m_Dirty);
            if (ImGui::Button("Save"))
                SaveBlueprintEdit(state);
            ImGui::SameLine();
            if (ImGui::Button("Revert"))
                LoadBlueprintEdit(state.m_Path, state);
            ImGui::EndDisabled();
            if (state.m_Dirty)
            {
                ImGui::SameLine();
                ImGui::TextDisabled("(unsaved changes)");
            }
            if (!state.m_Status.empty())
                ImGui::TextWrapped("%s", state.m_Status.c_str());
        }
    }

    void Editor::DrawBlueprintsMenu()
    {
        if (!m_ShowBlueprintsMenu)
            return;

        static bool error = false;
        const char* errorMsg = "An error occurred while accessing blueprints. Check logs";
        if (ImGui::Begin("Blueprints", &m_ShowBlueprintsMenu))
        {
            if (!m_CurrentProject)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "No project loaded");
                ImGui::End();
                return;
            }
            
            if (ImGui::IsWindowAppearing())
                error = false;
            if (error)
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorMsg);
            }
            static std::filesystem::path selectedBlueprint;
            static std::filesystem::path pendingSelection;
            static BlueprintEditState edit;
            static std::filesystem::path stateScope; // blueprints dir the static state belongs to

            const auto blueprintsDir = m_Services.projectManager->GetCurrentProjectPaths().Blueprints();

            // reset all selection/edit state when the active project changes so
            // state from one project can never leak into another
            if (stateScope != blueprintsDir)
            {
                if (edit.m_Loaded && edit.m_Dirty)
                {
                    SS_DEBUG_LOG("[BLUEPRINTS] Discarding unsaved edits to '" + edit.m_Path.string()
                                 + "' because the active project changed")
                }
                selectedBlueprint.clear();
                pendingSelection.clear();
                edit = BlueprintEditState{};
                stateScope = blueprintsDir;
            }

            try
            {
                if (std::filesystem::create_directory(blueprintsDir))
                {
                    SS_DEBUG_LOG("[BLUEPRINTS] Created blueprints directory")
                }

                // refresh the edit state from disk when the window reopens
                if (ImGui::IsWindowAppearing() && !selectedBlueprint.empty()
                    && std::filesystem::is_regular_file(selectedBlueprint))
                {
                    LoadBlueprintEdit(selectedBlueprint, edit);
                }

                if (ImGui::Button("New Blueprint"))
                    ImGui::OpenPopup("New Blueprint");

                CenterNext();
                if (ImGui::BeginPopupModal("New Blueprint", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    static char nameBuf[64];
                    static char folderBuf[256];
                    static std::string createError;
                    if (ImGui::IsWindowAppearing())
                    {
                        nameBuf[0] = '\0';
                        // prefill the folder from the current selection
                        std::string folder;
                        if (!selectedBlueprint.empty())
                            folder = std::filesystem::relative(selectedBlueprint.parent_path(), blueprintsDir).generic_string();
                        if (folder == ".")
                            folder.clear();
                        std::snprintf(folderBuf, sizeof(folderBuf), "%s", folder.c_str());
                        createError.clear();
                        ImGui::SetKeyboardFocusHere();
                    }

                    ImGui::InputText("Name", nameBuf, IM_ARRAYSIZE(nameBuf));
                    ImGui::InputText("Folder", folderBuf, IM_ARRAYSIZE(folderBuf));
                    ImGui::SameLine(); HelpMarker("Optional subfolder inside the blueprints directory. It will be created if it doesn't exist");
                    if (!createError.empty())
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", createError.c_str());

                    if (ImGui::Button("Create"))
                    {
                        if (nameBuf[0] == '\0')
                            createError = "A name is required";
                        else if (std::strpbrk(nameBuf, "/\\"))
                            createError = "The name cannot contain path separators";
                        else
                        {
                            const auto root = std::filesystem::weakly_canonical(blueprintsDir);
                            const auto targetDir = std::filesystem::weakly_canonical(blueprintsDir / folderBuf);
                            const auto rootStr = root.generic_string();
                            const auto targetStr = targetDir.generic_string();
                            if (targetStr != rootStr && targetStr.rfind(rootStr + "/", 0) != 0)
                                createError = "The folder must stay inside the blueprints directory";
                            else
                            {
                                const auto filePath = targetDir / (std::string(nameBuf) + ".json");
                                if (std::filesystem::exists(filePath))
                                    createError = "A blueprint with that name already exists there";
                                else
                                {
                                    std::filesystem::create_directories(targetDir);
                                    std::ofstream file(filePath, std::ios::out | std::ios::trunc);
                                    if (!file.is_open())
                                        createError = "Failed to create the blueprint file";
                                    else
                                    {
                                        const json blueprintJson = EntityBlueprint{ nameBuf, {} };
                                        file << blueprintJson.dump(4);
                                        file.close();
                                        selectedBlueprint = filePath;
                                        pendingSelection.clear();
                                        LoadBlueprintEdit(filePath, edit);
                                        ImGui::CloseCurrentPopup();
                                    }
                                }
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                // draw a table with the first column as the blueprint tree
                //  and the second column as the selected blueprint's editor
                if (ImGui::BeginTable("BlueprintsTable", 2))
                {
                    ImGui::TableSetupColumn("Blueprint");
                    ImGui::TableSetupColumn("Components");
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    std::filesystem::path clicked;
                    DrawBlueprintTree(blueprintsDir, selectedBlueprint, clicked);

                    // drop stale selection (e.g. file deleted or moved)
                    if (!selectedBlueprint.empty() && !std::filesystem::is_regular_file(selectedBlueprint))
                    {
                        selectedBlueprint.clear();
                        edit = BlueprintEditState{};
                    }

                    if (!clicked.empty() && clicked != selectedBlueprint)
                    {
                        if (edit.m_Loaded && edit.m_Dirty)
                            pendingSelection = clicked; // confirm before discarding edits
                        else
                        {
                            selectedBlueprint = clicked;
                            LoadBlueprintEdit(clicked, edit);
                        }
                    }

                    ImGui::TableSetColumnIndex(1);
                    if (selectedBlueprint.empty())
                        ImGui::TextDisabled("Select a blueprint to edit");
                    else
                        DrawBlueprintEditor(edit, *m_Services.entityManager);

                    ImGui::EndTable();
                }

                // confirm discarding unsaved edits when switching blueprints
                if (!pendingSelection.empty() && !ImGui::IsPopupOpen("Unsaved Changes"))
                    ImGui::OpenPopup("Unsaved Changes");
                CenterNext();
                if (ImGui::BeginPopupModal("Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("'%s' has unsaved changes.", edit.m_Path.filename().string().c_str());
                    if (ImGui::Button("Discard"))
                    {
                        selectedBlueprint = pendingSelection;
                        pendingSelection.clear();
                        LoadBlueprintEdit(selectedBlueprint, edit);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Keep Editing"))
                    {
                        pendingSelection.clear();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                SS_DEBUG_LOG("[BLUEPRINTS] Failed to access blueprint file: " + std::string(e.what()))
                error = true;
            }
        }
        ImGui::End();
    }
}
