#include "Editor.h"
#include "Core/EntityManager.h"
#include "Core/ResourceManager.h"
#include "Core/SceneManager.h"
#include "System/SystemManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    struct ResourceManagerNode; // Used for the resource manager table

    bool LoadScene = false;
    bool ViewEntities = true;
    bool ViewResources = false;

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

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        ImGui::Begin("Special-Spoon Editor", nullptr, window_flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Scene"))
            {
                if (ImGui::MenuItem("New")) {}
                if (ImGui::MenuItem("Load")) LoadScene = true;
                if (ImGui::MenuItem("Save")) {}
                if (ImGui::MenuItem("Save As")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Entity Manager"))
            {
                if (ImGui::MenuItem("Entity View")) ViewEntities = !ViewEntities;
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Resource Manager"))
            {
                if(ImGui::MenuItem("Resource View")) ViewResources = !ViewResources;
                ImGui::EndMenu();
            }

            if(ImGui::Button("Play")) 
                m_Play = true;
            if(ImGui::Button("Stop"))
                Stop();

            ImGui::EndMenuBar();
        }
        ImGui::SeparatorText("Object viewer");
        if (ViewEntities) ViewEntitiesMenu(e_Manager);
        
        ImGui::End();
        if (LoadScene) { LoadSceneMenu(e_Manager, s_Manager, sys_Manager); }
        if (ViewResources) { ViewResourcesMenu(); }
    }

    void Editor::LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
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
        ImGui::End();
    }

    void Editor::ViewEntitiesMenu(EntityManager& e_Manager)
    {
        // Display a selectable list of active entities
        const auto& entities = e_Manager.GetAllEntities();
        static UUID selectedID = 0;
        static std::string selectedComp = "";

        if (ImGui::BeginListBox("Entities"))
        {
            for (const auto& [uuid, name] : entities)
            {
                ImGui::PushID(name.c_str());
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

        Component* selectedComponent = nullptr;
        ImGui::BeginChild("Component View");
        if(ImGui::BeginListBox("Components"))
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

            const char* popupName = "Delete?";
            if(ImGui::Button("Delete")) ImGui::OpenPopup(popupName);

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if(ImGui::BeginPopupModal(popupName))
            {
                ImGui::Text("Are you sure you want to\ndelete this component? This cannot be undone!");
                if(ImGui::Button("Delete")) { e_Manager.KillComponent(selectedComponent->GetType(), selectedID); }
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
        ImGui::EndChild();
    }

    void Editor::ViewResourcesMenu()
    {
        ImGui::Begin("Resources");
        if(ImGui::BeginTable("Resource Manager", 3))
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