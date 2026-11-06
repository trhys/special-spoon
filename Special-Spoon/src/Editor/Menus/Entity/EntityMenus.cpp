#include "Editor/Editor.h"
#include "Editor/Utils/Windows.h"
#include "ECS/Blueprints/Blueprint.h"
#include "Core/Registers/Component/ComponentRegistry.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace Spoon
{
    void Editor::DrawEntityList()
    {
        if (!m_ShowEntityList)
            return;

        // Display a selectable list of active entities
        const auto& entities = m_Services.entityManager->GetAllEntities();
        static std::string selectedComp = "";

        ImGui::SeparatorText("Entities");
        if (ImGui::BeginListBox("##Entities"))
        {
            for (const auto& [uuid, name] : entities)
            {
                ImGui::PushID(uuid.ID);
                
                const bool is_selected = (m_SelectedEntity == uuid);
                
                std::string displayID = name == "" ? 
                "unknown" + std::to_string(uuid.ID)
                 : name + "--" + std::to_string(uuid.ID);

                if (ImGui::Selectable(displayID.c_str(), is_selected))
                    m_SelectedEntity = uuid;
                
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                ImGui::PopID();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of all active entities in the scene");
        }
        if (ImGui::Button("Add Entity"))
            m_ShowCreateEntity = true;
    }

    void Editor::DrawCreateEntity()
    {
        if (!m_ShowCreateEntity)
            return;

        static char newEntityBuf[64];
        static std::filesystem::path selectedBlueprint; // empty = create an empty entity
        static std::string blueprintError;
        auto clear = [&]()
        {
            newEntityBuf[0] = '\0';
            selectedBlueprint.clear();
            blueprintError.clear();
            m_ShowCreateEntity = false;
            ImGui::CloseCurrentPopup();
        };

        const char* popupName = "New Entity";
        CenterNext();
        if (!ImGui::IsPopupOpen(popupName))
            ImGui::OpenPopup(popupName);
        if (ImGui::BeginPopupModal(popupName))
        {
            if (ImGui::IsWindowAppearing())
            {
                selectedBlueprint.clear();
                blueprintError.clear();
            }

            ImGui::InputText("Entity Name", newEntityBuf, IM_ARRAYSIZE(newEntityBuf));
            ImGui::SameLine(); HelpMarker("It's recommended to give your entity some kind of name here. It's not required but"
                                          "will make life easier when you've got many entities in the scene. Entites will be"
                                          "managed by UUID so this name is only for visual reference in the editor");
            
            if (ImGui::BeginChild("Blueprint Selector", ImVec2(0, 200), ImGuiChildFlags_Borders))
            {
                if (ImGui::Selectable("<Empty Entity>", selectedBlueprint.empty()))
                    selectedBlueprint.clear();

                if (!m_CurrentProject)
                    ImGui::TextDisabled("Load a project to create entities from blueprints");
                else
                {
                    const auto blueprintsDir = m_Services.projectManager->GetCurrentProjectPaths().Blueprints();
                    try
                    {
                        std::vector<std::filesystem::path> blueprints;
                        if (std::filesystem::is_directory(blueprintsDir))
                        {
                            for (const auto& entry : std::filesystem::recursive_directory_iterator(blueprintsDir))
                            {
                                if (entry.is_regular_file() && entry.path().extension() == ".json")
                                    blueprints.push_back(entry.path());
                            }
                        }
                        std::sort(blueprints.begin(), blueprints.end());

                        for (const auto& blueprint : blueprints)
                        {
                            const std::string label = std::filesystem::relative(blueprint, blueprintsDir).generic_string();
                            if (ImGui::Selectable(label.c_str(), selectedBlueprint == blueprint))
                                selectedBlueprint = blueprint;
                        }
                    }
                    catch (const std::filesystem::filesystem_error& e)
                    {
                        SS_DEBUG_LOG("[BLUEPRINTS] Failed to list blueprints: " + std::string(e.what()))
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to list blueprints. Check logs");
                    }
                }
            }
            ImGui::EndChild();

            if (!blueprintError.empty())
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", blueprintError.c_str());

            ImGui::Separator();
            if (ImGui::Button("Submit"))
            {
                blueprintError.clear();
                if (selectedBlueprint.empty())
                {
                    m_Services.entityManager->CreateEntity(newEntityBuf);
                    clear();
                }
                else
                {
                    // parse the blueprint before creating anything so parse
                    // failures never leave a partial entity behind
                    try
                    {
                        std::ifstream file(selectedBlueprint);
                        if (!file.is_open())
                            throw std::runtime_error("failed to open blueprint file");

                        json blueprintJson;
                        file >> blueprintJson;
                        const EntityBlueprint blueprint = blueprintJson.get<EntityBlueprint>();

                        const std::string name = newEntityBuf[0] != '\0' ? newEntityBuf : blueprint.m_Name;
                        const UUID id = m_Services.entityManager->CreateEntity(name);
                        try
                        {
                            const auto& loaders = ComponentRegistry::Get().GetLoaders();
                            for (const auto& comp : blueprint.m_Components)
                            {
                                const auto loader = loaders.find(comp.m_Name);
                                if (loader == loaders.end())
                                    throw std::runtime_error("no loader registered for component type: " + comp.m_Name);
                                loader->second(*m_Services.entityManager, id, comp.m_Data);
                            }
                            clear();
                        }
                        catch (...)
                        {
                            // clean up the partially created entity before surfacing the failure
                            m_Services.entityManager->KillEntity(id);
                            throw;
                        }
                    }
                    catch (const std::exception& e) // json, filesystem and loader failures
                    {
                        SS_DEBUG_LOG("[BLUEPRINTS] Failed to instantiate blueprint '" + selectedBlueprint.string()
                                     + "': " + std::string(e.what()))
                        blueprintError = "Failed to instantiate blueprint: " + std::string(e.what());
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
                clear();
            ImGui::EndPopup();
        }
    }

    void Editor::DrawComponentList()
    {
        if (!m_ShowComponentList || !m_SelectedEntity.ID < 0)
            return;

        ImGui::SeparatorText("Component Inspector");
        if(ImGui::BeginListBox("##Components"))
        {
            for(auto comp : m_Services.entityManager->GetAllComponentsOfEntity(m_SelectedEntity))
            {
                ImGui::PushID(comp->GetDisplayName().c_str());
                const bool compSelected = (m_SelectedComponent == comp);
                if(ImGui::Selectable(comp->GetDisplayName().c_str(), compSelected))
                    m_SelectedComponent = comp;
                
                if (compSelected)
                    ImGui::SetItemDefaultFocus();

                ImGui::PopID();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of all components belonging to this entity");

            if(ImGui::Button("Add Component"))
            {
                if (m_Services.entityManager->GetAllEntities().empty())
                    ImGui::SetItemTooltip("You must select an entity before adding a new component!");
                else 
                    m_ShowAddComponent = true;
            }
            ImGui::SameLine();
            if(ImGui::Button("Delete") && m_SelectedComponent) 
                m_ShowDeleteComponent = true;
        }

        // individual component inspector
        // call the components onreflect() here (see Component.h)
        if(m_SelectedComponent)
        {
            ImGui::SeparatorText("Component Inspector");
            if(ImGui::BeginChild("##Component Inspector"))
                m_SelectedComponent->OnReflect();
            ImGui::EndChild();
        }
    }

    void Editor::DrawAddComponent()
    {
        if (!m_ShowAddComponent)
            return;

        auto finish = [this](){
            m_ShowAddComponent = false;
            ImGui::CloseCurrentPopup();
        };

        const char* compAdd = "Add Component";
        if(!ImGui::IsPopupOpen(compAdd))
            ImGui::OpenPopup(compAdd);
        
        static std::unordered_map<std::string, bool> compSelections;
        static ImGuiTextFilter filter;

        if(ImGui::BeginPopupModal(compAdd, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            // Clear selections/filter and set focus on first open
            if (ImGui::IsWindowAppearing())
            {
                compSelections.clear();
                filter.Clear();
                ImGui::SetKeyboardFocusHere();
            }

            // Filter box
            filter.Draw("Search");
            ImGui::SameLine(); HelpMarker("Type to filter components by name");
            ImGui::Separator();

            const auto& arrays = m_Services.entityManager->GetAllArrays();
            if (ImGui::BeginChild("##Available Components", ImVec2(0, 300)))
            {
                for (const auto& [type, array] : arrays)
                {
                    if (!filter.PassFilter(type.c_str()))
                        continue;

                    ImGui::PushID(type.c_str());
                    bool alreadyHas = array->HasEntity(m_SelectedEntity);
                    if (alreadyHas)
                        compSelections[type] = true;
                    ImGui::BeginDisabled(alreadyHas);
                    ImGui::Checkbox(type.c_str(), &compSelections[type]);
                    ImGui::EndDisabled();
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();
            
            if (ImGui::Button("Submit", ImVec2(120, 0)))
            {
                for (const auto& [type, array] : arrays)
                {
                    if (compSelections[type] && !array->HasEntity(m_SelectedEntity))
                        m_Services.entityManager->GetCreators().at(type)(m_SelectedEntity);
                }
                
                finish();
            }
            if(ImGui::Button("Cancel", ImVec2(120, 0)))
                finish();

            ImGui::EndPopup();
        }
    }

    void Editor::DrawDeleteComponent()
    {
        if (!m_ShowDeleteComponent)
            return;
        
        if (!m_SkipDeleteComponentPrompt) 
        {
            if (!ImGui::IsPopupOpen("Delete Component"))
                ImGui::OpenPopup("Delete Component");

            auto finish = [this](){
                m_SelectedComponent = nullptr;
                m_ShowDeleteComponent = false;
                ImGui::CloseCurrentPopup();
            };

            CenterNext();
            if(ImGui::BeginPopupModal("Delete Component") && !m_SkipDeleteComponentPrompt)
            {
                ImGui::Text("Are you sure you want to\ndelete this component? This cannot be undone!");
                ImGui::Checkbox("Don't ask me again", &m_SkipDeleteComponentPrompt);
                if(ImGui::Button("Delete")) 
                { 
                    m_Services.entityManager->KillComponent(m_SelectedComponent->GetDisplayName(), m_SelectedEntity);
                    finish();
                }
                ImGui::SameLine(); if(ImGui::Button("Cancel")) { finish(); }
                ImGui::EndPopup();
            }
        } else m_Services.entityManager->KillComponent(m_SelectedComponent->GetDisplayName(), m_SelectedEntity);
    }
}