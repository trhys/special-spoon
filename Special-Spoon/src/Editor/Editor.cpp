#include "Editor.h"

#include "Menus/SceneMenus.h"
#include "Menus/EntityMenus.h"
#include "Utils/EditorSettings.h"
#include "Utils/Helpmarker.h"

#include "Core/EntityManager.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "Core/SceneManager.h"
#include "Core/Serialization/Serializer.h"

#include "System/SystemManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    void Editor::Stop() 
    {
        m_Play = false;
    }

    bool Editor::Play()
    {
        return m_Play;
    }

    void Editor::Run(sf::Time tick, EntityManager& e_Manager, SceneManager& s_Manager, SystemManager& sys_Manager)
    {
        if (!workingDir)
            workingDir = ResourceManager::Get().GetAssetsDir();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scene Manager"))
            {
                if (ImGui::MenuItem("New")) NewScene = true;
                if (ImGui::MenuItem("Load")) LoadScene = true;
                if (ImGui::MenuItem("Save")) Serialize(*m_ActiveScene, e_Manager, sys_Manager);
                if (ImGui::MenuItem("Scene Manifest")) ImGui::SetTooltip("todo");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Entity Manager"))
            {
                if (ImGui::MenuItem("Entity View")) ViewEntities = !ViewEntities;
                if (ImGui::MenuItem("Entity Blueprints")) ImGui::SetTooltip("todo");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Resource Manager"))
            {
                if (ImGui::MenuItem("Resource Tree")) ViewResources = !ViewResources;
                if (ImGui::MenuItem("Load Resources")) LoadResources = !LoadResources;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Animation Tool"))
            {
                if (ImGui::MenuItem("Open Animation Tool")) { m_AnimationTool.Open(nullptr); } 
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("System Manager"))
            {
                if (ImGui::MenuItem("Open System Manager", nullptr, false, m_ActiveScene != nullptr)) m_SystemsMenu.Open();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::Checkbox("Confirm component delete", &EditorSettings::Get().compDelAskAgain);
                ImGui::EndMenu();
            }

            if (ImGui::Button("Play")) 
                m_Play = true;
            if (ImGui::Button("Stop"))
                Stop();

            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Special-Spoon Editor");
        if (m_ActiveScene)
        {
            ImGui::Text("Active Scene ID: %s", m_ActiveScene->ID.c_str());
            if (ViewEntities) ViewEntitiesMenu(e_Manager);
        }
        else
        {
            ImGui::Text("Load/Create a scene to view and create entities!");
        }

        ImGui::End();

        if (NewScene)       { NewSceneMenu(s_Manager, this); }
        if (LoadScene)      { LoadSceneMenu(e_Manager, s_Manager, sys_Manager, this); }
        if (ViewResources)  { ViewResourcesMenu(); }
        if (LoadResources)  { LoadResourcesMenu(); }

        if (m_AnimationTool.IsOpen()) m_AnimationTool.Update(tick);
        if (m_SystemsMenu.IsOpen())   m_SystemsMenu.Update(sys_Manager);
    }

    void Editor::ViewResourcesMenu()
    {
        ImGui::Begin("Resources", &ViewResources);
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
            workingDir = ResourceManager::Get().GetAssetsDir();
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
            if (ImGui::BeginPopupContextItem())
            {
                ResourceType type = ResourceManager::Get().GetType(node->m_Ext);
                bool supported = (type != ResourceType::Unknown);

                if (ImGui::Selectable("Add to Scene", false, supported ? 0 : ImGuiSelectableFlags_Disabled))
                {
                    switch (type)
                    {
                    case ResourceType::Texture: ResourceManager::Get().LoadResource<sf::Texture>(node->m_Name, node->m_Path); break;
                    case ResourceType::Font: ResourceManager::Get().LoadResource<sf::Font>(node->m_Name, node->m_Path); break;
                    case ResourceType::Sound: ResourceManager::Get().LoadResource<sf::SoundBuffer>(node->m_Name, node->m_Path); break;
                    default: break;
                    }
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Selectable("Remove from Scene", false, supported ? 0 : ImGuiSelectableFlags_Disabled))
                {
                    switch (type)
                    {
                    case ResourceType::Texture: ResourceManager::Get().RemoveResource<sf::Texture>(node->m_Name); break;
                    case ResourceType::Font: ResourceManager::Get().RemoveResource<sf::Font>(node->m_Name); break;
                    case ResourceType::Sound: ResourceManager::Get().RemoveResource<sf::SoundBuffer>(node->m_Name); break;
                    default: break;
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(node->m_Size.c_str());

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(node->m_Ext.c_str());

            
        }          
        ImGui::PopID();
    }

    void Editor::ShowTree(AssetNode* node)
    {
        ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags leaf_flags = base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::PushID(node->m_Path.string().c_str());
        if(node->isDir)
        {
            bool isOpen = ImGui::TreeNodeEx(node->m_Name.c_str(), base_flags);
            if(isOpen)
            {
                for (const auto& child : node->m_Children)
                {
                    ShowTree(child.get());
                }
                ImGui::TreePop();
            }
        }
        else
        {
            ResourceType type = ResourceManager::Get().GetType(node->m_Ext);
            bool supported = (type != ResourceType::Unknown);
            ImGui::TreeNodeEx(node->m_Name.c_str(), leaf_flags);
            if (supported && ImGui::BeginDragDropSource())
            {
                switch (type)
                    {
                    case ResourceType::Texture: ImGui::SetDragDropPayload("LOAD_TEXTURE", &node, sizeof(AssetNode*)); break;
                    case ResourceType::Font: ImGui::SetDragDropPayload("LOAD_FONT", &node, sizeof(AssetNode*)); break;
                    case ResourceType::Sound: ImGui::SetDragDropPayload("LOAD_SOUND", &node, sizeof(AssetNode*)); break;
                    default: break;
                    }
                ImGui::EndDragDropSource();
            }
        }
        ImGui::PopID();
    }

    void Editor::LoadResourcesMenu()
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Resource Loading Menu", &LoadResources);

        static std::string selTexture = "empty";
        static std::string selFont = "Default";
        static std::string selSound = "";

        float windowWidth = ImGui::GetContentRegionAvail().x;
        float listWidth = 150.0f;

        if (ImGui::BeginTable("Resources Menu", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
        {
            ImGui::TableNextColumn();
            ShowTree(workingDir);

            ImGui::TableNextColumn();
            if (ImGui::BeginChild("Textures", ImVec2(0, 250), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
            {
                if (ImGui::BeginListBox("##Loaded Textures", ImVec2(listWidth, -1)))
                {
                    for (const auto& [id, texture] : ResourceManager::Get().GetTextures())
                    {
                        if (ImGui::Selectable(id.c_str(), selTexture == id))
                        {
                            selTexture = id;
                        }
                    }
                    ImGui::EndListBox();
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LOAD_TEXTURE"))
                        {
                            AssetNode* assetPtr = *(AssetNode**)payload->Data;
                            ResourceManager::Get().LoadResource<sf::Texture>(assetPtr->m_Name, assetPtr->m_Path);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                ImGui::SameLine();

                // Get aspect ratio for texture preview
                sf::Texture& texture = ResourceManager::Get().GetResource<sf::Texture>(selTexture);
                ImGui::Image(texture, GetAspectRatio(texture));
                ImGui::EndChild();
            }

            if (ImGui::BeginChild("Fonts", ImVec2(0, 250), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
            {
                if (ImGui::BeginListBox("##Loaded Fonts", ImVec2(listWidth, -1)))
                {
                    for (const auto& [id, font] : ResourceManager::Get().GetFonts())
                    {
                        if (ImGui::Selectable(id.c_str(), selFont == id))
                        {
                            selFont = id;
                        }
                    }
                    ImGui::EndListBox();
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LOAD_FONT"))
                        {
                            AssetNode* assetPtr = *(AssetNode**)payload->Data;
                            ResourceManager::Get().LoadResource<sf::Font>(assetPtr->m_Name, assetPtr->m_Path);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                ImGui::SameLine(); 

                // Get aspect ratio for font preview
                sf::Texture& texture = ResourceManager::Get().GetFontPreview(selFont);
                ImGui::Image(texture, GetAspectRatio(texture));
                ImGui::EndChild();
            }

            if (ImGui::BeginChild("Sounds", ImVec2(0, 250), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
            {
                if (ImGui::BeginListBox("Loaded Sounds", ImVec2(listWidth, -1)))
                {
                    for (const auto& [id, sound] : ResourceManager::Get().GetSounds())
                    {
                        //if (ImGui::Selectable(id.c_str()))
                        //{
                        //    //todo
                        //}
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }

    void Editor::EditTextureRect(SpriteComp& comp)
    {
        m_TextureRectTool.Run(comp);
    }
}
