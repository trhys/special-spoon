#include "Editor.h"
#include "Core/EntityManager.h"
#include "Core/ResourceManager.h"
#include "Core/SceneManager.h"
#include "System/SystemManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    static void HelpMarker(const char* desc);

    struct ResourceManagerNode; // Used for the resource manager table

    bool LoadScene = false;
    bool ViewEntities = true;

    // ===================================================================

    void Editor::Stop() 
    {
        m_Play = false;
    }

    bool Editor::Play()
    {
        m_Play = true;
        return m_Play;
    }

    void Editor::Run(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
        ImGui::Begin("Special-Spoon Editor");

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Scene"))
            {
                if(ImGui::MenuItem("New")) {}
                if(ImGui::MenuItem("Load")) LoadScene = true;
                if(ImGui::MenuItem("Save")) {}
                if(ImGui::MenuItem("Save As")) {}
            }

            if(ImGui::BeginMenu("Entity Manager"))
            {
                if(ImGui::MenuItem("Entity Viewer")) ViewEntities = !ViewEntities;
            }

            ImGui::EndMenuBar();
            if(LoadScene) { LoadSceneMenu(e_Manager, s_Manager, sys_Manager); }
        }

        ImGui::SeparatorText("Scene object view");
        if(ViewEntities) ViewEntitiesMenu(e_Manager);

        ImGui::End();
    }

    void Editor::LoadSceneMenu(EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
        ImGui::Begin("Load Scene", &LoadScene);
        const auto& scenes = s_Manager.GetManifest();
        static std::string selectedScene = "";

        ImGui::BeginListBox("Scene Manifest");
        for(const auto& [name, sceneData] : scenes)
        {
            const bool is_selected = (selectedScene == name);
            if(ImGui::Selectable(name.c_str(), is_selected))
                selectedScene = name;

            if(is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
        ImGui::SameLine(); HelpMarker("A list of available scenes in the manifest");

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
        static std::string selectedName = "";

        ImGui::BeginListBox("Entities");
        for(const auto& [uuid, name] : entities)
        {
            const bool is_selected = (selectedID == uuid);
            if (ImGui::Selectable(name.c_str(), is_selected))
            {
                selectedID = uuid;
                selectedName = name;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
        ImGui::SameLine(); HelpMarker("A list of all active entities in the scene");

        ImGui::BeginChild("Component View");
        if(ImGui::TreeNodeEx(selectedName.c_str()))
        {
            for(const auto comp : e_Manager.GetAllComponentsOfEntity(selectedID))
            {
                ImGui::PushID(comp.c_str());
                const char* popupName = "Delete?";
                if(ImGui::TreeNodeEx(comp.c_str()));
                if(ImGui::Button("Delete")) ImGui::OpenPopup(popupName);
                
                // Always center this window when appearing
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                if(ImGui::BeginPopupModal(popupName))
                {
                    ImGui::Text("Are you sure you want to delete this component? This cannot be undone! // todo: undo/redo");
                    if(ImGui::Button("Delete")) { e_Manager.KillComponent(comp, selectedID); }
                    ImGui::SameLine(); if(ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }
                    ImGui::EndPopup();
                }
                ImGui::PopID();
                ImGui::TreePop();
            }
            if(ImGui::Button("Add component")) {}
        }
        ImGui::TreePop();
        ImGui::EndChild();
    }

    void Editor::ViewResourcesMenu()
    {
        ImGui::Begin("Resources");
        if(ImGui::BeginTable("Resource Manager"))
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

            bool openTextures = ImGui::TreeNodeEx("Textures", ImGuiTreeNodeFlags_SpanFullWidth);

            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");
            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");

            if(openTextures)
            {
                for(const auto& [id, file] : ResourceManager::GetTextureMap())
                {
                    auto sizeInBytes = std::filesystem::file_size(file);
                    std::string sizeStr = std::to_string(sizeInBytes);

                    ImGui::PushID(id.c_str());
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TreeNodeEx(id.c_str(), childFlags);
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(sizeStr.c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Image file");
                    
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }

    // ============================================================

    // struct ResourceManagerNode
    // {
    //     const char*     Name;
    //     const char*     Type;
    //     int             Size;
    //     int             ChildIndex;
    //     int             ChildCount;
    // }

    static void HelpMarker(const char* desc)
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