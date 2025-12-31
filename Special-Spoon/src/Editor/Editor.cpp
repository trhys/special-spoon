#include "Editor.h"
#include "Blueprints/Blueprint.h"
#include "Core/EntityManager.h"
#include "Core/ResourceManager/ResourceManager.h"
#include "Core/Serialization/Serializer.h"
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

    // Settings
    bool compDelAskAgain = true;

    // ===================================================================

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
                ImGui::Checkbox("Confirm component delete", &compDelAskAgain);
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

        if (NewScene)       { NewSceneMenu(s_Manager); }
        if (LoadScene)      { LoadSceneMenu(e_Manager, s_Manager, sys_Manager); }
        if (ViewResources)  { ViewResourcesMenu(); }
        if (LoadResources)  { LoadResourcesMenu(); }

        if (m_AnimationTool.IsOpen()) m_AnimationTool.Update(tick);
        if (m_SystemsMenu.IsOpen())   m_SystemsMenu.Update(sys_Manager);
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
                m_ActiveScene = s_Manager.CreateScene(newSceneBuf);
                newSceneBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
                NewScene = false;
            }
            if(ImGui::Button("Submit") && strlen(newSceneBuf) > 0 )
            {
                m_ActiveScene = s_Manager.CreateScene(newSceneBuf);
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
            m_ActiveScene = s_Manager.LoadScene(selectedScene, e_Manager, sys_Manager);
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
                if (selectedBP->GetDisplayName() != "None")
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
            if(ImGui::BeginPopupModal(popupName) && compDelAskAgain)
            {
                ImGui::Text("Are you sure you want to\ndelete this component? This cannot be undone!");
                ImGui::Checkbox("Don't ask me again", &compDelAskAgain);
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
                for (const auto& [type, creator] : manager.GetCreators())
                {
                    ImGui::Checkbox(type.c_str(), &compSelections[type]);
                }
                ImGui::EndChild();
            }
            
            if (ImGui::Button("Submit"))
            {
                const auto& arrays = manager.GetAllArrays();
                for (const auto& [type, selected] : compSelections)
                {
                    bool alreadyExists = arrays.at(type)->HasEntity(id);
                    if(selected && !alreadyExists) manager.GetCreators().at(type)(id);
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
                ImVec2 space = ImGui::GetContentRegionAvail();
                sf::Vector2u texSize = texture.getSize();
                float aspect = (float)texSize.x / (float)texSize.y;
                float displayWidth = space.x;
                float displayHeight = space.x / aspect;
                if (displayHeight > space.y) 
                {
                    displayHeight = space.y;
                    displayWidth = space.y * aspect;
                }

                ImGui::Image(texture, ImVec2(displayWidth, displayHeight));
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
                ImVec2 space = ImGui::GetContentRegionAvail();
                sf::Vector2u texSize = texture.getSize();
                float aspect = (float)texSize.x / (float)texSize.y;
                float displayWidth = space.x;
                float displayHeight = space.x / aspect;
                if (displayHeight > space.y) 
                {
                    displayHeight = space.y;
                    displayWidth = space.y * aspect;
                }
                
                ImGui::Image(texture, ImVec2(displayWidth, displayHeight));
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
