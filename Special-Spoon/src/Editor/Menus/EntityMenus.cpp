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

    void SelectEntity(UUID id)
    {
        static UUID selectedID = id;
    }
    
    void ViewEntitiesMenu(EntityManager& e_Manager)
    {
        // Display a selectable list of active entities
        const auto& entities = e_Manager.GetAllEntities();
        static UUID selectedID = 0;
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
                ImGui::PopID();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of all active entities in the scene");
        }
        if (ImGui::Button("Add Entity"))
        {
            ImGui::OpenPopup("New Entity");
        }
        
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("New Entity"))
        {
            static const Blueprint* selectedBP = nullptr;
            static char newEntityBuf[64];
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
                newEntityBuf[0] = '\0';
                selectedBP = nullptr;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                newEntityBuf[0] = '\0';
                selectedBP = nullptr;
                ImGui::CloseCurrentPopup();
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

        if(ImGui::BeginPopupModal(compAdd))
        {
            if (ImGui::BeginChild("Available Components", ImVec2(0, 200)))
            {
                for (const auto& [type, creator] : manager.GetCreators())
                {
                    ImGui::Checkbox(type.c_str(), &compSelections[type]);
                }
                ImGui::EndChild();
            }
            
            if (ImGui::Button("Submit"))
            {
                const auto& arrays = manager.GetAllArrays();
                for (const auto& [type, array] : arrays)
                {
                    if (compSelections[array->GetDisplayName()])
                    {
                        bool alreadyExists = array->HasEntity(id);
                        if (!alreadyExists) manager.GetCreators().at(array->GetDisplayName())(id);
                    }
                }
                
                AddingComponent = false;
                ImGui::CloseCurrentPopup();
            }
            if(ImGui::Button("Cancel"))
            {
                AddingComponent = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}