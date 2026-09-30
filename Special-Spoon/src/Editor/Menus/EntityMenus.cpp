#include "EntityMenus.h"
#include "Editor/Overlay.h"
#include "Editor/Utils/EditorSettings.h"
#include "Editor/Utils/Helpmarker.h"
#include "Editor/Blueprints/Blueprint.h"

#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"
#include "Core/Project/ProjectManager.h"
#include "ECS/Components/World/TileMapComp.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

#include <algorithm>
#include <cstdio>
#include <iterator>

namespace Spoon
{
    static bool AddingComponent = false;
    static UUID selectedID = 0;
    static bool changedSelection = false;

    // Sets the currently selected entity in the editor
    // and sets the selection rectangle to size of the
    // entities sprite/text, or to a default if neither exist
    void SelectEntity(UUID id, Editor* editor, EntityManager& manager)
    {
        selectedID = id;
        changedSelection = true;

        auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
        auto& textArray = manager.GetArray<TextComp>(TextComp::Name);
        if (spriteArray.m_IdToIndex.count(id))
        {
            SpriteComp& sprite = manager.GetComponent<SpriteComp>(id, SpriteComp::Name);
            sf::FloatRect bounds = sprite.m_Sprite.getGlobalBounds();
            editor->m_SelectionRect.setPosition(sf::Vector2f(bounds.position.x, bounds.position.y));
            editor->m_SelectionRect.setSize(sf::Vector2f(bounds.size.x, bounds.size.y));
        }
        else if (textArray.m_IdToIndex.count(id))
        {
            TextComp& text = manager.GetComponent<TextComp>(id, TextComp::Name);
            sf::FloatRect bounds = text.m_Text.getGlobalBounds();
            editor->m_SelectionRect.setPosition(sf::Vector2f(bounds.position.x, bounds.position.y));
            editor->m_SelectionRect.setSize(sf::Vector2f(bounds.size.x, bounds.size.y));
        }
        else
            editor->m_SelectionRect.setSize(sf::Vector2f(100.f, 100.f)); // Default size
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
        static Blueprint selectedBP = Blueprint{"None", {}};
        static char newEntityBuf[64];
        auto clear = [&]()
        {
            newEntityBuf[0] = '\0';
            selectedBP = Blueprint{"None", {}};
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
                if (ImGui::BeginCombo("##Blueprints", selectedBP.GetDisplayName().c_str()))
                {
                    const auto& blueprints = Application::Get().GetProjectManager().GetBlueprints();
                    for (std::size_t index = 0; index < blueprints.size(); ++index)
                    {
                        const Blueprint& blueprint = blueprints[index];
                        const std::string& name = blueprint.GetDisplayName();
                        ImGui::PushID(static_cast<int>(index));
                        if (ImGui::Selectable(name.empty() ? "<Unnamed Blueprint>" : name.c_str(), selectedBP == blueprint))
                        {
                            selectedBP = blueprint;
                        }
                        ImGui::PopID();
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
                for (const auto& compID : selectedBP.GetComps())
                {
                    e_Manager.GetCreators().at(compID)(id);
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
            if(ImGui::BeginPopupModal(popupName) && !EditorSettings::Get().skipAskBeforeDeleteComp)
            {
                ImGui::Text("Are you sure you want to\ndelete this component? This cannot be undone!");
                ImGui::Checkbox("Don't ask me again", &EditorSettings::Get().skipAskBeforeDeleteComp);
                if(ImGui::Button("Delete")) 
                { 
                    e_Manager.KillComponent(selectedComponent->GetDisplayName(), selectedID);
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

                // todo: copilot put this here --- it goes in the components implementation onreflect()
                if (selectedComponent->GetDisplayName() == TileMapComp::Name)
                {
                    if (ImGui::Button("Open Tile Map Editor"))
                        Application::Get().GetEditor().EditTileMap(selectedID);
                }

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
                        auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
                        if (type == ColliderComp::Name && spriteArray.m_IdToIndex.count(id))
                        {
                            ColliderComp& collider = manager.GetComponent<ColliderComp>(id, ColliderComp::Name);
                            SpriteComp& sprite = manager.GetComponent<SpriteComp>(id, SpriteComp::Name);
                            collider.SetAABBSize(sprite.GetBoundingBox().size);
                        }
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

    // create/edit prefab blueprints
    void BlueprintsMenu(EntityManager& manager)
    {
        static std::string selectedBlueprintID;
        static char blueprintName[64] = "";
        static std::unordered_map<std::string, bool> componentSelections;
        static ImGuiTextFilter componentFilter;
        auto& blueprints = Application::Get().GetProjectManager().GetBlueprints();

        ImGui::SetNextWindowSize(ImVec2(760, 520), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Blueprint Creator"))
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginTable(
            "##BlueprintLayout",
            2,
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Blueprints", ImGuiTableColumnFlags_WidthFixed, 220.0f);
            ImGui::TableSetupColumn("Editor", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::SeparatorText("Existing Blueprints");
            if (ImGui::Button("New Blueprint", ImVec2(-FLT_MIN, 0)))
            {
                selectedBlueprintID.clear();
                blueprintName[0] = '\0';
                componentSelections.clear();
                componentFilter.Clear();
            }

            if (ImGui::BeginChild("##BlueprintList", ImVec2(0, 0), ImGuiChildFlags_Borders))
            {
                for (std::size_t index = 0; index < blueprints.size(); ++index)
                {
                    const Blueprint& blueprint = blueprints[index];
                    const std::string& name = blueprint.GetDisplayName();
                    const bool selected = selectedBlueprintID == name;
                    ImGui::PushID(static_cast<int>(index));
                    if (ImGui::Selectable(name.empty() ? "<Unnamed Blueprint>" : name.c_str(), selected))
                    {
                        selectedBlueprintID = name;
                        std::snprintf(blueprintName, IM_ARRAYSIZE(blueprintName), "%s", name.c_str());
                        componentSelections.clear();
                        for (const std::string& component : blueprint.GetComps())
                        {
                            componentSelections[component] = true;
                        }
                        componentFilter.Clear();
                    }
                    if (selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1);
            const auto selectedBlueprint = std::find_if(
                blueprints.begin(),
                blueprints.end(),
                [&](const Blueprint& blueprint)
                {
                    return blueprint.GetDisplayName() == selectedBlueprintID;
                }
            );
            const std::size_t selectedBlueprintIndex =
                selectedBlueprint == blueprints.end()
                    ? blueprints.size()
                    : static_cast<std::size_t>(std::distance(blueprints.begin(), selectedBlueprint));
            const bool editing = selectedBlueprintIndex < blueprints.size();
            ImGui::SeparatorText(editing ? "Edit Blueprint" : "Create Blueprint");
            ImGui::InputText("Name", blueprintName, IM_ARRAYSIZE(blueprintName));

            ImGui::Spacing();
            ImGui::TextUnformatted("Components");
            componentFilter.Draw("Search##BlueprintComponents", -FLT_MIN);

            if (ImGui::BeginChild("##BlueprintComponents", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2.0f), ImGuiChildFlags_Borders))
            {
                for (const auto& [type, array] : manager.GetAllArrays())
                {
                    if (!componentFilter.PassFilter(type.c_str()))
                        continue;

                    ImGui::PushID(type.c_str());
                    ImGui::Checkbox(type.c_str(), &componentSelections[type]);
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();

            const std::string enteredName(blueprintName);
            const bool duplicateName = std::any_of(
                blueprints.begin(),
                blueprints.end(),
                [&](const Blueprint& blueprint)
                {
                    return blueprint.GetDisplayName() == enteredName &&
                        (!editing || &blueprint != &blueprints[selectedBlueprintIndex]);
                }
            );
            const bool canSave = !enteredName.empty() && !duplicateName;

            ImGui::BeginDisabled(!canSave);
            const bool saveBlueprint =
                ImGui::Button(editing ? "Save Changes" : "Create Blueprint", ImVec2(140, 0));
            ImGui::EndDisabled();
            if (saveBlueprint)
            {
                std::vector<std::string> compsToAdd;
                for (const auto& [comp, selected] : componentSelections)
                {
                    if (selected)
                    {
                        compsToAdd.push_back(comp);
                    }
                }

                if (editing)
                {
                    auto& blueprint = blueprints[selectedBlueprintIndex];
                    blueprint.SetName(enteredName);
                    blueprint.SetComponents(compsToAdd);
                }
                else
                {
                    blueprints.emplace_back(enteredName, compsToAdd);
                }
                selectedBlueprintID = enteredName;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(100, 0)))
            {
                selectedBlueprintID.clear();
                blueprintName[0] = '\0';
                componentSelections.clear();
                componentFilter.Clear();
            }

            if (editing)
            {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                if (ImGui::Button("Delete", ImVec2(100, 0)))
                {
                    blueprints.erase(selectedBlueprint);
                }
                ImGui::PopStyleColor();
            }
            if (enteredName.empty())
                ImGui::TextDisabled("Blueprint name cannot be empty.");
            else if (duplicateName)
                ImGui::TextDisabled("Blueprint names must be unique.");

            ImGui::EndTable();
        }

        ImGui::End();
    }

}
