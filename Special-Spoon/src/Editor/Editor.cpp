#include "Editor.h"
#include "Core/EntityManager.h"
#include "Core/ResourceManager.h"
#include "Core/SceneManager.h"
#include "System/SystemManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    // Menu flags
    bool NewScene = false;
    bool LoadScene = false;
    bool ViewEntities = true;
    bool ViewResources = false;
    bool LoadResources = false;
    bool AddingComponent = false;

    // ===================================================================

    void Editor::Stop() 
    {
        m_Play = false;
    }

    bool Editor::Play()
    {
        return m_Play;
    }

    void Editor::Run(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
        if(!workingDir)
            workingDir = ResourceManager::GetAssetsDir();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("New")) NewScene = true;
                if (ImGui::MenuItem("Load")) LoadScene = true;
                if (ImGui::MenuItem("Save")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Entity Manager"))
            {
                if (ImGui::MenuItem("Entity View")) ViewEntities = !ViewEntities;
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Resource Manager"))
            {
                if(ImGui::MenuItem("Resource Tree")) ViewResources = !ViewResources;
                if (ImGui::MenuItem("Load Resources")) LoadResources = true;
                ImGui::EndMenu();
            }

            if(ImGui::Button("Play")) 
                m_Play = true;
            if(ImGui::Button("Stop"))
                Stop();

            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Special-Spoon Editor");
        if (ViewEntities) ViewEntitiesMenu(e_Manager);
        
        ImGui::End();

        if (NewScene) { NewSceneMenu(s_Manager); }
        if (LoadScene) { LoadSceneMenu(e_Manager, s_Manager, sys_Manager); }
        if (ViewResources) { ViewResourcesMenu(); }
    }

    void Editor::NewSceneMenu(SceneManager& s_Manager)
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        const char* popupId = "New Scene";
        static char newSceneBuf[64] = {0};
        if (!ImGui::IsPopupOpen(popupId))
            ImGui::OpenPopup(popupId);

        if(ImGui::BeginPopupModal(popupId, &NewScene, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter scene ID: "); ImGui::SameLine();

            if (ImGui::IsWindowAppearing())
                ImGui::SetKeyboardFocusHere();

            if(ImGui::InputText("##ID", newSceneBuf, IM_ARRAYSIZE(newSceneBuf), ImGuiInputTextFlags_EnterReturnsTrue) && strlen(newSceneBuf) > 0 )
            {
                s_Manager.CreateScene(newSceneBuf);
                newSceneBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
                NewScene = false;
            }
            if(ImGui::Button("Submit") && strlen(newSceneBuf) > 0 )
            {
                s_Manager.CreateScene(newSceneBuf);
                newSceneBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
                NewScene = false;
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                newSceneBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
                NewScene = false;
            }
            ImGui::EndPopup();
        }
    }

    void Editor::LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        ImGui::Begin("Load Scene", &LoadScene);
        const auto& scenes = s_Manager.GetManifest();
        static std::string selectedScene = "";

        if (ImGui::BeginListBox("Scene Manifest"))
        {
            for (const auto& [name, sceneData] : scenes)
            {
                const bool is_selected = (selectedScene == name);
                if (ImGui::Selectable(name.c_str(), is_selected))
                    selectedScene = name;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
            ImGui::SameLine(); HelpMarker("A list of available scenes in the manifest");
        }
        
        if(ImGui::Button("Open"))
        {
            s_Manager.LoadScene(selectedScene, e_Manager, sys_Manager);
            LoadScene = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")) 
            LoadScene = false;
        
        ImGui::End();
    }

    void Editor::ViewEntitiesMenu(EntityManager& e_Manager)
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
                if (ImGui::Selectable(name.c_str(), is_selected))
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
        
        if (ImGui::BeginPopup("New Entity"))
        {
            static char newEntityBuf[64];
            ImGui::InputText("Entity Name", newEntityBuf, IM_ARRAYSIZE(newEntityBuf));
            if (ImGui::Button("Submit"))
            {
                e_Manager.CreateEntity(newEntityBuf);
                newEntityBuf[0] = '\0';
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
                selectedComp = "";
                AddingComponent = true;
            }
            if (AddingComponent)
                AddComponentMenu(selectedID, e_Manager);

            ImGui::SameLine();
            const char* popupName = "Delete?";
            if(ImGui::Button("Delete")) 
                ImGui::OpenPopup(popupName);

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if(ImGui::BeginPopupModal(popupName))
            {
                ImGui::Text("Are you sure you want to\ndelete this component? This cannot be undone!");
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

    void Editor::AddComponentMenu(UUID id, EntityManager& manager)
    {
        const char* compAdd = "Add Component";
        if(!ImGui::IsPopupOpen(compAdd))
            ImGui::OpenPopup(compAdd);
        static std::unordered_map<std::string, bool> compSelections;

        if(ImGui::BeginPopupModal(compAdd))
        {
            if (ImGui::BeginChild("Available Components", ImVec2(0, 200)))
            {
                for (const auto& [type, array] : manager.GetAllArrays())
                {
                    ImGui::Checkbox(array->GetDisplayName().c_str(), &compSelections[type]);
                }
                ImGui::EndChild();
            }
            
            if (ImGui::Button("Submit"))
            {
                for (const auto& [type, selected] : compSelections)
                {
                    if(selected) manager.GetCreators().at(type)(id);
                    AddingComponent = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            if(ImGui::Button("Cancel"))
            {
                AddingComponent = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void Editor::ViewResourcesMenu()
    {
        ImGui::Begin("Resources");
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg 
            | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable 
            | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
        if(ImGui::BeginTable("Resource Manager", 3), flags)
        {
            static ImGuiTreeNodeFlags childFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;

            // Setup table
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Size");
            ImGui::TableSetupColumn("Type");
            ImGui::TableHeadersRow();

            // Populate
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ViewAssets(workingDir);
            ImGui::EndTable();
        }

        if(ImGui::Button("Refresh"))
            workingDir = ResourceManager::GetAssetsDir();
        if(ImGui::Button("Close"))
            ViewResources = false;

        ImGui::End();
    }

    void Editor::ViewAssets(AssetNode* node)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::PushID(node->m_Path.string().c_str());

        ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags leaf_flags = base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if(node->isDir)
        {
            bool isOpen = ImGui::TreeNodeEx(node->m_Name.c_str(), base_flags);

            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");
            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");

            if(isOpen)
            {
                for (const auto& child : node->m_Children)
                {
                    ViewAssets(child.get());
                }
                ImGui::TreePop();
            }
        }
        else
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TreeNodeEx(node->m_Name.c_str(), leaf_flags);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(node->m_Size.c_str());

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(node->m_Ext.c_str());
        }          
        ImGui::PopID();
    }

    void Editor::LoadResourcesMenu()
    {

    }

    void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}
