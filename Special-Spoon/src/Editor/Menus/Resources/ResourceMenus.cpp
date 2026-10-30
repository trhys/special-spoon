#include "Editor/Editor.h"
#include "Core/ResourceManager/ResourceManager.h"

namespace Spoon
{
    void Editor::DrawResourcesMenu()
    {
        if (!m_ShowResourcesMenu)
            return;

        ImGui::Begin("Resource Menu", &m_ShowResourcesMenu, ImGuiWindowFlags_MenuBar);
        if (!workingDir)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::Text("ERROR: No working directory selected.");
            ImGui::PopStyleColor();
            ImGui::End();
            return;
        }

        // begin menu bar
        if (ImGui::BeginMenuBar())
        {
            // back navigation
            if (ImGui::Button("<-"))
            {
                if (workingDir && workingDir->m_Parent)
                    workingDir = workingDir->m_Parent;
            }

            // refresh
            ImGui::BeginDisabled(!m_CurrentProject);
            if (ImGui::Button("Refresh"))
            {
                ResourceManager::Get().RefreshAssets(m_CurrentProject->assetsPath);
                workingDir = ResourceManager::Get().GetAssetsDir();
            }
            ImGui::EndDisabled();

            ImGui::EndMenuBar();
        }

        // draw table of current dir
        ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedSame;
        if (ImGui::BeginTable("ResourceTable", 6, flags))
        {   
            // iterate node children
            int column = 0;
            for (auto& child : workingDir->m_Children)
            {
                // todo: make a config
                ImGui::TableNextColumn();
                column++;
                if (column >= 6)
                {
                    ImGui::TableNextRow();
                    column = 0;
                }

                const char* name = child->m_Name.c_str();

                if (child->isDir)
                {
                    sf::Texture& folder = ResourceManager::Get().GetResource<sf::Texture>("special-spoon-folder-icon");
                    ImGui::ImageButton(name, folder.getNativeHandle(), ImVec2(128, 128));
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        workingDir = child.get();
                    }
                }
                else
                {
                    // branch on resource/file type
                    // use context menus to provide additional actions for each resource type
                    ResourceType type = ResourceManager::Get().GetType(child->m_Ext);
                    switch (type)
                    {
                        case ResourceType::Texture:
                        {
                            sf::Texture& texture = ResourceManager::Get().GetThumbnail(child.get());
                            if (ImGui::ImageButton(name, texture.getNativeHandle(), ImVec2(128, 128)))
                            {
                                // handle texture click
                            }
                            if (ImGui::BeginPopupContextItem(child->m_Name.c_str()))
                            {
                                if (ImGui::MenuItem("Add to Scene"))
                                    ResourceManager::Get().LoadResource<sf::Texture>(child->m_Name, child->m_Path);
                                if (ImGui::MenuItem("Remove from Scene"))
                                    ResourceManager::Get().RemoveResource<sf::Texture>(child->m_Name);
                                ImGui::EndPopup();
                            }
                            break;
                        }
                        case ResourceType::Sound:
                            // todo: replace nullptr with actual sound icon
                            if (ImGui::ImageButton(name, nullptr, ImVec2(128, 128)))
                            {
                                // handle sound click
                            }

                            if (ImGui::BeginPopupContextItem(child->m_Name.c_str()))
                            {
                                if (ImGui::MenuItem("Add to Scene"))
                                    ResourceManager::Get().LoadResource<sf::SoundBuffer>(child->m_Name, child->m_Path);
                                if (ImGui::MenuItem("Remove from Scene"))
                                    ResourceManager::Get().RemoveResource<sf::SoundBuffer>(child->m_Name);
                                ImGui::EndPopup();
                            }
                            break;
                        case ResourceType::Font:
                        {
                            sf::Texture& preview = ResourceManager::Get().GetFontPreview(child->m_Name);
                            if (ImGui::ImageButton(name, preview.getNativeHandle(), ImVec2(128, 128)))
                            {
                                // handle font click
                            }
                            if (ImGui::BeginPopupContextItem(child->m_Name.c_str()))
                            {
                                if (ImGui::MenuItem("Add to Scene"))
                                    ResourceManager::Get().LoadResource<sf::Font>(child->m_Name, child->m_Path);
                                if (ImGui::MenuItem("Remove from Scene"))
                                    ResourceManager::Get().RemoveResource<sf::Font>(child->m_Name);
                                ImGui::EndPopup();
                            }
                            break;
                        }
                        default:
                            // todo: replace nullptr with actual default icon
                            if (ImGui::ImageButton(name, nullptr, ImVec2(128, 128)))
                            {
                                // handle default click
                            }
                            break;
                    }
                }
                ImGui::TextWrapped(child->m_Name.c_str());
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }
}