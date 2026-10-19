#include "Editor/Editor.h"
#include "Editor/Utils/Windows.h"

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

        static Blueprint selectedBP = Blueprint{"None", {}};
        static char newEntityBuf[64];
        auto clear = [&]()
        {
            newEntityBuf[0] = '\0';
            selectedBP = Blueprint{"None", {}};
            m_ShowCreateEntity = false;
            ImGui::CloseCurrentPopup();
        };

        const char* popupName = "New Entity";
        CenterNext();
        if (!ImGui::IsPopupOpen(popupName))
            ImGui::OpenPopup(popupName);
        if (ImGui::BeginPopupModal(popupName))
        {
            ImGui::InputText("Entity Name", newEntityBuf, IM_ARRAYSIZE(newEntityBuf));
            ImGui::SameLine(); HelpMarker("It's recommended to give your entity some kind of name here. It's not required but"
                                          "will make life easier when you've got many entities in the scene. Entites will be"
                                          "managed by UUID so this name is only for visual reference in the editor");
            
            if (ImGui::BeginChild("Blueprint Selector", ImVec2(0, 200), ImGuiChildFlags_Borders))
            {
                if (ImGui::BeginCombo("##Blueprints", selectedBP.GetDisplayName().c_str()))
                {
                    const auto& blueprints = m_Services.projectManager->GetBlueprints();
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
            }
            ImGui::EndChild();
            ImGui::Separator();
            if (ImGui::Button("Submit"))
            {
                UUID id = m_Services.entityManager->CreateEntity(newEntityBuf);
                for (const auto& compID : selectedBP.GetComps())
                {
                    m_Services.entityManager->GetCreators().at(compID)(id);
                }
                clear();
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
                ImGui::EndChild();
            }
            
            if (ImGui::Button("Submit", ImVec2(120, 0)))
            {
                for (const auto& [type, array] : arrays)
                {
                    if (compSelections[type] && !array->HasEntity(m_SelectedEntity))
                    {
                        m_Services.entityManager->GetCreators().at(type)(m_SelectedEntity);

                        // todo: get this terrible block out of here
                        // got to think of a better way
                        auto& spriteArray = m_Services.entityManager->GetArray<SpriteComp>(SpriteComp::Name);
                        if (type == ColliderComp::Name && spriteArray.m_IdToIndex.count(m_SelectedEntity))
                        {
                            ColliderComp& collider = m_Services.entityManager->GetComponent<ColliderComp>(m_SelectedEntity, ColliderComp::Name);
                            SpriteComp& sprite = m_Services.entityManager->GetComponent<SpriteComp>(m_SelectedEntity, SpriteComp::Name);
                            collider.SetAABBSize(sprite.GetBoundingBox().size);
                        }
                    }
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