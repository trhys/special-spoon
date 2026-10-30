#include "AnimationTool.h"

#include <algorithm>
#include <cmath>

namespace Spoon
{
    void AnimationTool::Update(sf::Time tick)
    {
        static bool OpenAnimation = false;

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Animation Tool", &m_isOpen, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Open"))
                {
                    if (ImGui::MenuItem("Load Animation")) OpenAnimation = true;
                    if (ImGui::MenuItem("New Animation")) createModal = true;
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Settings"))
                {
                    ImGui::Checkbox("Autoplay animation", &autoPlayEnabled);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) 
                    {
                        ImGui::SetTooltip("Checked if you want animations to automatically play when loaded");
                    }

                    ImGui::Checkbox("Show tooltips", &showToolTips);
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) 
                    {
                        ImGui::SetTooltip("Checked if you want to display tooltips when hovering");
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::Button("Play"))
                {
                    if (isFinished)
                    {
                        isFinished = false;
                        currentFrame = 0;
                        elapsedTime = 0.0f;
                    }
                    m_Playback = true;
                }
                if (ImGui::Button("Stop"))
                {
                    m_Playback = false;
                }
                ImGui::EndMenuBar();
            }
            if (OpenAnimation)
            {
                ImGui::OpenPopup("Open Animation Data");
                OpenAnimation = false;
            }
            if (createModal) CreateNew();

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Open Animation Data"))
            {
                for (auto& [id, animation] : ResourceManager::Get().GetAnimations())
                {
                    if (ImGui::Selectable(id.c_str()))
                    {
                        Open(ResourceManager::Get().GetAnimationData(id));
                        ImGui::CloseCurrentPopup();
                    }
                }
                if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::BeginTable("##animtool", 2, ImGuiTableFlags_BordersV))
            {
                ImGui::TableNextColumn();
                if (ImGui::BeginChild("AT Viewport", ImVec2(640, 640), ImGuiChildFlags_Borders))
                {
                    RenderViewport(m_MainVP);
                    if (m_Playback) Animate(tick);
                    else
                    {
                        if (!currentData->spriteCords.empty())
                        {
                            SpriteCords& sc = currentData->spriteCords[currentFrame];
                            previewSprite.SetTextureRect(sf::IntRect({ sc.x, sc.y }, { sc.width, sc.height }));
                        }
                        else previewSprite.SetTextureRect(sf::IntRect());
                        
                    }
                    m_MainVP.target.clear(sf::Color(50, 50, 50));
                    m_MainVP.target.draw(previewSprite.m_Sprite);
                    m_MainVP.target.display();
                    ImGui::Image(m_MainVP.target);
                }
                ImGui::EndChild();
                ImGui::Checkbox("Loop Animation", &m_Looping);
                ImGui::SameLine(); HelpMarker("Checked if you want the animation to continue playing when finished");

                ImGui::TableNextColumn();
                if (currentData)
                {
                    if (ImGui::BeginChild("##Viewport", ImVec2(0, 600), ImGuiChildFlags_Borders))
                    {
                        RenderViewport(m_EditorVP);

                        sf::RectangleShape rectPreview;
                        rectPreview.setSize({ (float)rect.size.x, (float)rect.size.y });
                        rectPreview.setPosition({ (float)rect.position.x, (float)rect.position.y });
                        rectPreview.setFillColor(sf::Color::Transparent);
                        rectPreview.setOutlineColor(sf::Color::Red);
                        rectPreview.setOutlineThickness(1.0);

                        m_EditorVP.target.clear(sf::Color(50, 50, 50));
                        m_EditorVP.target.draw(editorSprite.m_Sprite);
                        sf::RenderStates selectionStates;
                        selectionStates.transform = editorSprite.m_Sprite.getTransform();
                        m_EditorVP.target.draw(rectPreview, selectionStates);
                        m_EditorVP.target.display();

                        ImGui::Image(m_EditorVP.target);
                        ImVec2 viewportPos = ImGui::GetItemRectMin();
                        
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && ImGui::IsItemHovered())
                        {
                            ImVec2 mousePos = ImGui::GetIO().MousePos;
                            sf::Vector2i relativeMouse(
                                static_cast<int>(mousePos.x - viewportPos.x),
                                static_cast<int>(mousePos.y - viewportPos.y)
                            );
                            sf::Vector2f worldMouse = m_EditorVP.target.mapPixelToCoords(relativeMouse);
                            sf::Vector2f localMouse = editorSprite.m_Sprite.getInverseTransform().transformPoint(worldMouse);
                            const sf::Vector2i sheetSize = editorSprite.m_Sprite.getTextureRect().size;
                            sf::Vector2i textureMouse(
                                static_cast<int>(std::floor(std::clamp(localMouse.x, 0.0f, static_cast<float>(sheetSize.x)))),
                                static_cast<int>(std::floor(std::clamp(localMouse.y, 0.0f, static_cast<float>(sheetSize.y))))
                            );
                            if (!m_Dragging)
                            {
                                m_DragStart = textureMouse;
                                m_Dragging = true;
                            }
                            rect.position.x = std::min(m_DragStart.x, textureMouse.x);
                            rect.position.y = std::min(m_DragStart.y, textureMouse.y);
                            rect.size.x = std::abs(textureMouse.x - m_DragStart.x);
                            rect.size.y = std::abs(textureMouse.y - m_DragStart.y);
                        }
                        else m_Dragging = false;

                    }
                    ImGui::EndChild();

                    ImGui::Separator();
                    const bool selectionValid = rect.size.x > 0 && rect.size.y > 0;
                    ImGui::BeginDisabled(!selectionValid);
                    if (ImGui::Button("Add Frame"))
                    {
                        SpriteCords cords(
                            rect.position.x,
                            rect.position.y,
                            rect.size.x,
                            rect.size.y
                        );
                        currentData->spriteCords.push_back(cords);
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled) && showToolTips) 
                        ImGui::SetTooltip("Adds a new frame from drawn preview rect. This is pushed to the back of the loop");
                    ImGui::EndDisabled();

                    ImGui::SameLine();
                    ImGui::BeginDisabled(currentData->spriteCords.empty());

                    ImGui::BeginDisabled(!selectionValid);
                    if (ImGui::Button("Update Frame"))
                    {
                        SpriteCords cords(
                            rect.position.x,
                            rect.position.y,
                            rect.size.x,
                            rect.size.y
                        );
                        currentData->spriteCords[frameIndex] = cords;
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled) && showToolTips) 
                        ImGui::SetTooltip("Updates current selected frame index");
                    ImGui::EndDisabled();

                    ImGui::SameLine();
                    if (ImGui::Button("Delete Frame"))
                    {
                        auto& cords = currentData->spriteCords;
                        cords.erase(cords.begin() + frameIndex);
                        if (currentFrame >= (int)cords.size() && !cords.empty())
                        {
                            currentFrame = (int)cords.size() - 1;
                        }
                        frameIndex = currentFrame;
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled) && showToolTips) 
                        ImGui::SetTooltip("Deletes current selected frame index");

                    if (ImGui::SliderInt("Frame index", &frameIndex, 0, (int)currentData->spriteCords.size() - 1))
                    {
                        m_Playback = false;
                        currentFrame = frameIndex;
                    }
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled) && showToolTips) 
                        ImGui::SetTooltip("Selects active frame. Modifying this will end playback");

                    if (ImGui::InputFloat("Framerate", &currentData->frameRate, 0.01f, 1.0f)) {}
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled) && showToolTips) 
                        ImGui::SetTooltip("Updates the animation frame rate. Higher value slows the playback speed. Hold ctrl while incrementing to increase by 1");

                    ImGui::EndDisabled();
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void AnimationTool::Open(AnimationData* data)
    {
        currentData = data;
        currentFrame = 0;
        frameIndex = 0;
        elapsedTime = 0.0f;
        isFinished = false;
        m_isOpen = true;
        rect = sf::IntRect();
        m_Dragging = false;

        if (currentData && ResourceManager::Get().GetTextures().count(data->textureID))
        {
            previewSprite.m_Sprite.setTexture(ResourceManager::Get().GetResource<sf::Texture>(data->textureID), true);
            previewSprite.m_Sprite.setPosition(m_MainVP.camera.getCenter());
            editorSprite.m_Sprite.setTexture(ResourceManager::Get().GetResource<sf::Texture>(data->textureID), true);
            editorSprite.m_Sprite.setPosition(m_EditorVP.camera.getCenter());
            if (!currentData->spriteCords.empty())
            {
                SpriteCords& sc = currentData->spriteCords[0];
                previewSprite.SetTextureRect(sf::IntRect({ sc.x, sc.y }, { sc.width, sc.height }));
            }
            previewSprite.CenterOrigin();
        }
        else
        {
            previewSprite.m_Sprite.setTexture(ResourceManager::Get().GetResource<sf::Texture>("empty"), true);
            previewSprite.m_Sprite.setPosition(m_MainVP.camera.getCenter());
            editorSprite.m_Sprite.setTexture(ResourceManager::Get().GetResource<sf::Texture>("empty"), true);
            editorSprite.m_Sprite.setPosition(m_EditorVP.camera.getCenter());
        }
        m_Playback = autoPlayEnabled;
    }

    void AnimationTool::Animate(sf::Time tick)
    {
        // This block is pretty much completely copy/pasted from the renderer with
        // just a few unnecessary lines cut out 

        if(currentData != nullptr && !isFinished && !currentData->spriteCords.empty())
        {
            elapsedTime += tick.asSeconds();
            while(elapsedTime >= currentData->frameRate)
            {
                elapsedTime -= currentData->frameRate;
                currentFrame++;
            }
            if(currentFrame >= static_cast<int>(currentData->spriteCords.size()))
            {
                if(m_Looping)
                {
                    currentFrame = 0;
                }
                else
                {
                    currentFrame = static_cast<int>(currentData->spriteCords.size()) - 1;
                    isFinished = true;
                }
            }
            SpriteCords& sc = currentData->spriteCords[currentFrame];
            previewSprite.SetTextureRect(sf::IntRect({sc.x, sc.y}, {sc.width, sc.height}));
        }
        else {}
    }

    void AnimationTool::CreateNew()
    {
        const char* selectorPopup = "Texture Selector";
        if (!ImGui::IsPopupOpen(selectorPopup))
            ImGui::OpenPopup(selectorPopup);

        static char newAnimDatabuf[64];

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal(selectorPopup))
        {
            ImGui::Text("Select a texture");
            ImGui::Separator();
            static std::filesystem::path selectedPath;
            const auto library = ResourceManager::Get().GetAssetLibrary("Image file");
            AssetNode* selectedNode = nullptr;
            for (auto* node : library)
            {
                if (node->m_Path == selectedPath)
                {
                    selectedNode = node;
                    break;
                }
            }
            if (ImGui::BeginChild("Texture Explorer", ImVec2(0, 200), ImGuiChildFlags_Borders))
            {
                for (const auto& node : library)
                {
                    ImGui::PushID(node->m_Name.c_str());
                    if (ImGui::ImageButton(node->m_Name.c_str(), ResourceManager::Get().GetThumbnail(node), ImVec2(128, 128)))
                    {
                        selectedNode = node;
                        selectedPath = node->m_Path;
                    }
                    ImGui::SameLine();
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();

            if (ImGui::BeginChild("Preview", ImVec2(0, 600), ImGuiChildFlags_Borders))
            {
                if (selectedNode)
                {
                    ImGui::Text("Selected texture id: %s", selectedNode->m_Name.c_str());

                    // Get aspect ratio for texture preview
                    sf::Texture& texture = ResourceManager::Get().GetThumbnail(selectedNode);
                    ImGui::Image(texture, GetAspectRatio(texture));
                }
                else
                {
                    ImGui::Text("No texture selected");
                }
            }
            ImGui::EndChild();
            if (ImGui::InputText("Animation ID", newAnimDatabuf, IM_ARRAYSIZE(newAnimDatabuf), ImGuiInputTextFlags_CharsNoBlank)) {}
            bool bufferValid = newAnimDatabuf[0] != '\0';
            ImGui::BeginDisabled(!bufferValid || !selectedNode);
            if (ImGui::Button("Confirm"))
            {
                std::string selectedID = selectedNode->m_Name;
                if (!ResourceManager::Get().GetTextures().count(selectedID))
                {
                    ResourceManager::Get().LoadResource<sf::Texture>(selectedNode->m_Name, selectedNode->m_Path);
                }
                AnimationData data;
                data.ID = newAnimDatabuf;
                data.textureID = selectedID;
                ResourceManager::Get().LoadAnimationData(data.ID, data);
                Open(ResourceManager::Get().GetAnimationData(data.ID));

                newAnimDatabuf[0] = '\0';
                selectedPath.clear();
                createModal = false;
                editTool = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                newAnimDatabuf[0] = '\0';
                selectedPath.clear();
                createModal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void AnimationTool::Shutdown()
    {
        currentData = nullptr;
        m_MainVP.target = sf::RenderTexture();
        m_EditorVP.target = sf::RenderTexture();
    }
}