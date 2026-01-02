#include "ResourceMenus.h"
#include "Editor/Editor.h"
#include "Core/ResourceManager/ResourceManager.h"

#include "Imgui/imgui.h"
#include "Imgui-sfml/imgui-SFML.h"

namespace Spoon
{
    void ViewAssets(AssetNode* node)
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

    void ShowTree(AssetNode* node)
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

    void ViewResourcesMenu(Editor* editor)
    {
        ImGui::Begin("Resources", &editor->ViewResources);
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

            ViewAssets(editor->GetWorkingDir());
            ImGui::EndTable();
        }

        if(ImGui::Button("Refresh"))
            editor->SetWorkingDir(ResourceManager::Get().GetAssetsDir());
        if(ImGui::Button("Close"))
            editor->ViewResources = false;

        ImGui::End();
    }

    void LoadResourcesMenu(Editor* editor)
    {
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Resource Loading Menu", &editor->LoadResources);

        static std::string selTexture = "empty";
        static std::string selFont = "Default";
        static std::string selSound = "none";

        float windowWidth = ImGui::GetContentRegionAvail().x;
        float listWidth = 150.0f;

        if (ImGui::BeginTable("Resources Menu", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
        {
            ImGui::TableNextColumn();
            ShowTree(editor->GetWorkingDir());

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
                        if (ImGui::Selectable(id.c_str(), selSound == id))
                        {
                           selSound = id;
                        }
                    }
                    ImGui::EndListBox();
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LOAD_SOUND"))
                        {
                            AssetNode* assetPtr = *(AssetNode**)payload->Data;
                            ResourceManager::Get().LoadResource<sf::SoundBuffer>(assetPtr->m_Name, assetPtr->m_Path);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }
}