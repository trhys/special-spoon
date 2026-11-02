#include "AnimationTool.h"
#include "ECS/Components/Animation/AnimationData.h"
#include "Editor/Utils/Windows.h"

#include <algorithm>
#include <cmath>
#include <fstream>

namespace Spoon
{
    void AnimationTool::Update(sf::Time tick)
    {
        CenterNext();
        if (ImGui::Begin("Animation Tool", &m_isOpen, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Open"))
                {
                    if (ImGui::MenuItem("Load Animation")) showLoad = !showLoad;
                    if (ImGui::MenuItem("New Animation")) showCreate = !showCreate;
                    if (ImGui::MenuItem("Import Animation")) showImport = !showImport;
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

            DrawCreateNew();
            DrawLoadModal();
            DrawImportModal();

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
                        bool viewportHovered = ImGui::IsItemHovered();
                        
                        // grid slicer controls
                        DrawGridSlicerTools();
                        DrawGridOverlay(viewportPos);

                        // handle viewport interactions
                        HandleViewport(viewportPos, viewportHovered);
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
                    ImGui::BeginDisabled(currentData->spriteCords.empty() || !selectionValid);
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

                    // grid range addition
                    if (toolMode == ToolMode::Grid)
                    {
                        ImGui::SameLine();
                        ImGui::BeginDisabled(!CanUseGrid());
                        if (ImGui::Button("Add Grid Range"))
                        {
                            AddGridRangeFrames();
                        }
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled) && showToolTips)
                            ImGui::SetTooltip("Adds multiple frames starting from Grid Start for Grid Count.");
                        ImGui::EndDisabled();
                    }

                    ImGui::SameLine();
                    ImGui::BeginDisabled(currentData->spriteCords.empty());
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

    void AnimationTool::Shutdown()
    {
        currentData = nullptr;
        m_MainVP.target = sf::RenderTexture();
        m_EditorVP.target = sf::RenderTexture();
    }

    void AnimationTool::HandleViewport(const ImVec2& viewportPos, bool hovered)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && hovered)
        {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            sf::Vector2i relativeMouse(
                static_cast<int>(mousePos.x - viewportPos.x),
                static_cast<int>(mousePos.y - viewportPos.y)
            );
            sf::Vector2f worldMouse = m_EditorVP.target.mapPixelToCoords(relativeMouse);
            sf::Vector2f localMouse = editorSprite.m_Sprite.getInverseTransform().transformPoint(worldMouse);
            if (toolMode == ToolMode::Grid)
            {      
                sf::Vector2i texMouse(
                    static_cast<int>(std::floor(localMouse.x)),
                    static_cast<int>(std::floor(localMouse.y))
                );

                const std::optional<int> idx = m_Grid.IndexAtPixel(texMouse);
                if (idx.has_value())
                {
                    m_SelectedGridIndex = *idx;
                    ApplyGridSelectionToRect();
                }
            }
            else
            {
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
        }
        else m_Dragging = false;
    }

    // grid slicer tools
    void AnimationTool::DrawGridSlicerTools()
    {
        if (!currentData) return;

        ImGui::SeparatorText("Frame Selection");
        int mode = static_cast<int>(toolMode);
        ImGui::RadioButton("Manual Rect", &mode, static_cast<int>(ToolMode::Manual)); ImGui::SameLine();
        ImGui::RadioButton("Grid Slice", &mode, static_cast<int>(ToolMode::Grid));
        toolMode = static_cast<ToolMode>(mode);

        if (toolMode != ToolMode::Grid) return;

        ImGui::SeparatorText("Grid Slice Settings");
        ImGui::InputInt("Cell W", &m_Grid.cell.x);
        ImGui::InputInt("Cell H", &m_Grid.cell.y);
        ImGui::InputInt("Columns", &m_Grid.size.x);
        ImGui::InputInt("Rows", &m_Grid.size.y);
        ImGui::InputInt("Margin", &m_Grid.margin);
        ImGui::InputInt("Spacing", &m_Grid.spacing);
        ImGui::Checkbox("Show Grid Overlay", &m_ShowGridOverlay);
        ImGui::Checkbox("Clamp Rects To Texture", &m_GridClampToTexture);
        ImGui::Checkbox("Append Row-Major", &m_GridAppendRowMajor);

        if (m_Grid.cell.x < 0) m_Grid.cell.x = 0;
        if (m_Grid.cell.y < 0) m_Grid.cell.y = 0;
        if (m_Grid.size.x < 0) m_Grid.size.x = 0;
        if (m_Grid.size.y < 0) m_Grid.size.y = 0;
        if (m_Grid.margin < 0) m_Grid.margin = 0;
        if (m_Grid.spacing < 0) m_Grid.spacing = 0;

        const int count = m_Grid.Count();
        if (count <= 0)
        {
            m_SelectedGridIndex = -1;
            m_GridRangeStart = 0;
            m_GridRangeCount = 1;
        }
        else
        {
            if (m_SelectedGridIndex >= count) m_SelectedGridIndex = count - 1;
            if (m_SelectedGridIndex < -1) m_SelectedGridIndex = -1;
            m_GridRangeStart = std::clamp(m_GridRangeStart, 0, count - 1);
            m_GridRangeCount = std::clamp(m_GridRangeCount, 1, count);
        }

        ImGui::InputInt("Grid Start", &m_GridRangeStart);
        ImGui::InputInt("Grid Count", &m_GridRangeCount);
        if (count > 0)
        {
            m_GridRangeStart = std::clamp(m_GridRangeStart, 0, count - 1);
            m_GridRangeCount = std::clamp(m_GridRangeCount, 1, count - m_GridRangeStart);
        }
    }

    void AnimationTool::DrawGridOverlay(const ImVec2& viewportPos)
    {
        if (toolMode != ToolMode::Grid) return;
        if (!CanUseGrid()) return;

        const sf::Texture& texture = ResourceManager::Get().GetResource<sf::Texture>(currentData->textureID);
        const sf::Vector2u texSize = texture.getSize();

        if (!m_ShowGridOverlay) return;

        // Draw overlay in texture-local space onto the editor target.
        sf::RectangleShape outline;
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineThickness(1.0f);

        for (int i = 0; i < m_Grid.Count(); ++i)
        {
            const std::optional<sf::IntRect> r = m_Grid.RectForIndex(i);
            if (!r.has_value()) continue;

            sf::IntRect rectPx = *r;
            if (m_GridClampToTexture)
            {
                const int maxW = static_cast<int>(texSize.x);
                const int maxH = static_cast<int>(texSize.y);
                if (rectPx.position.x >= maxW || rectPx.position.y >= maxH) continue;
                rectPx.size.x = std::max(0, std::min(rectPx.size.x, maxW - rectPx.position.x));
                rectPx.size.y = std::max(0, std::min(rectPx.size.y, maxH - rectPx.position.y));
                if (rectPx.size.x <= 0 || rectPx.size.y <= 0) continue;
            }

            outline.setPosition({ static_cast<float>(rectPx.position.x), static_cast<float>(rectPx.position.y) });
            outline.setSize({ static_cast<float>(rectPx.size.x), static_cast<float>(rectPx.size.y) });
            outline.setOutlineColor(i == m_SelectedGridIndex ? sf::Color::Yellow : sf::Color(255, 255, 255, 90));
            sf::RenderStates states;
            states.transform = editorSprite.m_Sprite.getTransform();
            m_EditorVP.target.draw(outline, states);
        }
    }

    bool AnimationTool::CanUseGrid() const
    {
        if (!currentData) return false;
        if (!m_Grid.IsValid()) return false;
        if (!ResourceManager::Get().GetTextures().count(currentData->textureID)) return false;
        return true;
    }

    void AnimationTool::ApplyGridSelectionToRect()
    {
        if (!CanUseGrid()) return;
        if (m_SelectedGridIndex < 0) return;

        const std::optional<sf::IntRect> r = m_Grid.RectForIndex(m_SelectedGridIndex);
        if (!r.has_value()) return;

        rect = *r;
        if (m_GridClampToTexture && currentData)
        {
            const sf::Texture& texture = ResourceManager::Get().GetResource<sf::Texture>(currentData->textureID);
            const sf::Vector2u texSize = texture.getSize();
            rect.size.x = std::max(0, std::min(rect.size.x, static_cast<int>(texSize.x) - rect.position.x));
            rect.size.y = std::max(0, std::min(rect.size.y, static_cast<int>(texSize.y) - rect.position.y));
        }
    }


    void AnimationTool::AddGridRangeFrames()
    {
        if (!CanUseGrid()) return;
        if (!currentData) return;

        const sf::Texture& texture = ResourceManager::Get().GetResource<sf::Texture>(currentData->textureID);
        const sf::Vector2u texSize = texture.getSize();
        const int count = m_Grid.Count();
        if (count <= 0) return;

        const int start = std::clamp(m_GridRangeStart, 0, count - 1);
        const int len = std::clamp(m_GridRangeCount, 1, count - start);

        for (int i = 0; i < len; ++i)
        {
            const int idx = m_GridAppendRowMajor ? (start + i) : (start + i); // reserved hook if custom order added later
            const std::optional<sf::IntRect> maybe = m_Grid.RectForIndex(idx);
            if (!maybe.has_value()) continue;
            sf::IntRect r = *maybe;

            if (m_GridClampToTexture)
            {
                if (r.position.x >= static_cast<int>(texSize.x) || r.position.y >= static_cast<int>(texSize.y))
                    continue;
                r.size.x = std::max(0, std::min(r.size.x, static_cast<int>(texSize.x) - r.position.x));
                r.size.y = std::max(0, std::min(r.size.y, static_cast<int>(texSize.y) - r.position.y));
            }
            if (r.size.x <= 0 || r.size.y <= 0) continue;

            currentData->spriteCords.emplace_back(r.position.x, r.position.y, r.size.x, r.size.y);
        }
    }

    void AnimationTool::ImportJSON(
        AssetNode* selectedNode, 
        const std::filesystem::path& jsonPath
    )
    {
        if (!selectedNode || jsonPath.empty()) return;

        // load file path into json
        std::ifstream file(jsonPath, std::ios::in);
        if (!file.is_open()) return;

        nlohmann::json j;
        file >> j;
        file.close();

        // populate animation data from json
        for (auto& [key, animation] : j.items())
        {
            AnimationData animData;
            animData.ID = key;
            animData.textureID = selectedNode->m_Name;
            animData.spriteCords = animation.at("spriteCords").get<std::vector<SpriteCords>>();
            animData.frameRate = animation.value("frameRate", 0.1f);
            animData.isLooping = animation.value("isLooping", true);

            // validate frame rate
            if (animData.frameRate <= 0.0f)
                animData.frameRate = 0.1f;
            
            SaveAnimationData(animData, m_AnimationsDir);
        }
    }
}