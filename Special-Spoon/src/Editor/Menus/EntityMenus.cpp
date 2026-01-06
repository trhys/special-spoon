#include "EntityMenus.h"
#include "Editor/Utils/EditorSettings.h"
#include "Editor/Utils/Helpmarker.h"
#include "Editor/Blueprints/Blueprint.h"

#include "Core/EntityManager/EntityManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    static bool AddingComponent = false;
    static UUID selectedID = 0;
    static bool changedSelection = false;

    void SelectEntity(UUID id)
    {
        selectedID = id;
        changedSelection = true;
    }
    
    void ViewEntitiesMenu(EntityManager& e_Manager)
    {
        // Display a selectable list of active entities
        const auto& entities = e_Manager.GetAllEntities();
        static std::string selectedComp = "";

        ImGui::SeparatorText("Entities");
        if (ImGui::BeginListBox("##Entities"))
        {
            for (const auto& [uuid, name] : entities)
            {
                ImGui::PushID(uuid.ID);
                const bool is_selected = (selectedID == uuid);
                std::string displayID = name + "--" + std::to_string(uuid.ID);
                if (ImGui::Selectable(displayID.c_str(), is_selected))
                {
                    selectedID = uuid;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
                if (selectedID == uuid && changedSelection)
                {
                    ImGui::SetScrollHereY();
                    changedSelection = false;
                }
                ImGui::PopID();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of all active entities in the scene");
        }
        if (ImGui::Button("Add Entity"))
        {
            ImGui::OpenPopup("New Entity");
        }
        
        // Static variables for new entity popup +
        // Helper lambda to clear and close popup
        static const Blueprint* selectedBP = nullptr;
        static char newEntityBuf[64];
        auto clear = [&]()
        {
            newEntityBuf[0] = '\0';
            selectedBP = nullptr;
            ImGui::CloseCurrentPopup();
        };

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("New Entity"))
        {
            ImGui::InputText("Entity Name", newEntityBuf, IM_ARRAYSIZE(newEntityBuf));
            ImGui::SameLine(); HelpMarker("It's recommended to give your entity some kind of name here. It's not required but"
                                          "will make life easier when you've got many entities in the scene. Entites will be"
                                          "managed by UUID so this name is only for visual reference in the editor");
            
            if (ImGui::BeginChild("Blueprint Selector", ImVec2(0, 200), ImGuiChildFlags_Borders))
            {
                if (ImGui::BeginCombo("##Blueprints", selectedBP ? selectedBP->GetDisplayName().c_str() : "None"))
                {
                    for (const auto* blueprint : GetBlueprints())
                    {
                        if (ImGui::Selectable(blueprint->GetDisplayName().c_str(), selectedBP == blueprint))
                        {
                            selectedBP = blueprint;
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine(); HelpMarker("Optionally, select a blueprint to use for this entity. The selected blueprint will"
                                              "create the components automatically for that blueprint. See the tooltip for default"
                                              "blueprints for more info.");
                ImGui::EndChild();
            }
            ImGui::Separator();
            if (ImGui::Button("Submit"))
            {
                UUID id = e_Manager.CreateEntity(newEntityBuf);
                if (selectedBP)
                {
                    for (const auto& compID : selectedBP->GetComps())
                    {
                        e_Manager.GetCreators().at(compID)(id);
                    }
                }
                clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                clear();
            }
            ImGui::EndPopup();
        }
        
        ImGui::SeparatorText("Component Inspector");
        Component* selectedComponent = nullptr;
        if(ImGui::BeginListBox("##Components"))
        {
            for(auto comp : e_Manager.GetAllComponentsOfEntity(selectedID))
            {
                ImGui::PushID(comp->GetDisplayName().c_str());
                const bool compSelected = (selectedComp == comp->GetDisplayName());
                if(ImGui::Selectable(comp->GetDisplayName().c_str(), compSelected))
                {
                    selectedComp = comp->GetDisplayName();
                }
                if (compSelected)
                {
                    selectedComponent = comp;
                    ImGui::SetItemDefaultFocus();
                }
                ImGui::PopID();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of all components belonging to this entity");

            if(ImGui::Button("Add Component"))
            {
                if (e_Manager.GetAllEntities().empty())
                    ImGui::SetItemTooltip("You must select an entity before adding a new component!");
                else 
                {
                    selectedComp = "";
                    AddingComponent = true;
                }
            }
            if (AddingComponent)
                AddComponentMenu(selectedID, e_Manager);

            ImGui::SameLine();
            const char* popupName = "Delete?";
            if(ImGui::Button("Delete")) 
                ImGui::OpenPopup(popupName);

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if(ImGui::BeginPopupModal(popupName) && EditorSettings::Get().compDelAskAgain)
            {
                ImGui::Text("Are you sure you want to\ndelete this component? This cannot be undone!");
                ImGui::Checkbox("Don't ask me again", &EditorSettings::Get().compDelAskAgain);
                if(ImGui::Button("Delete")) 
                { 
                    e_Manager.KillComponent(selectedComponent->GetType(), selectedID);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine(); if(ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
        }
        if(selectedComponent)
        {
            ImGui::SeparatorText("Inspector");
            if(ImGui::BeginChild("Component Inspector"))
            {
                selectedComponent->OnReflect();
                ImGui::EndChild();
            }
        }
    }

    void AddComponentMenu(UUID& id, EntityManager& manager)
    {
        const char* compAdd = "Add Component";
        if(!ImGui::IsPopupOpen(compAdd))
            ImGui::OpenPopup(compAdd);
        static std::unordered_map<std::string, bool> compSelections;
        static ImGuiTextFilter filter;

        if(ImGui::BeginPopupModal(compAdd, &AddingComponent, ImGuiWindowFlags_AlwaysAutoResize))
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

            const auto& arrays = manager.GetAllArrays();
            if (ImGui::BeginChild("##Available Components", ImVec2(0, 300)))
            {
                for (const auto& [type, array] : arrays)
                {
                    if (!filter.PassFilter(type.c_str()))
                        continue;

                    ImGui::PushID(type.c_str());
                    bool alreadyHas = array->HasEntity(id);
                    if (alreadyHas)
                        compSelections[type] = true;
                    ImGui::BeginDisabled(alreadyHas);
                    ImGui::Checkbox(type.c_str(), &compSelections[type]);
                    ImGui::EndDisabled();
                    ImGui::PopID();
                }
                ImGui::EndChild();
            }
            
            if (ImGui::Button("Submit", ImVec2(120, 0)))
            {
                for (const auto& [type, array] : arrays)
                {
                    if (compSelections[type] && !array->HasEntity(id))
                    {
                        manager.GetCreators().at(type)(id);
                    }
                }
                
                AddingComponent = false;
                ImGui::CloseCurrentPopup();
            }
            if(ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                AddingComponent = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}