#include "AnimationTool.h"

namespace Spoon
{
    void AnimationTool::Update(sf::Time tick)
    {
        static bool OpenAnimation = false;

        if (ImGui::Begin("Animation Tool", &m_isOpen, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Open"))
                {
                    if (ImGui::Selectable("Open Animation"))
                    {
                        OpenAnimation = true;
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

            if (ImGui::BeginChild("AT Viewport", ImVec2(640, 640), ImGuiChildFlags_Borders))
            {
                // Resize viewport if necessary
                sf::Vector2f viewportSize = ImGui::GetContentRegionAvail();
                sf::Vector2u viewport2u(
                    std::max(1u, static_cast<unsigned int>(viewportSize.x)),
                    std::max(1u, static_cast<unsigned int>(viewportSize.y)));
                if (m_Viewport.getSize() != viewport2u)
                    if (m_Viewport.resize({ viewport2u }))
                    {
                        sf::View view(sf::FloatRect({ 0.f, 0.f }, { (float)viewport2u.x, (float)viewport2u.y }));
                        m_Viewport.setView(view);
                    }

                if (m_Playback) Animate(tick);

                m_Viewport.clear(sf::Color(50, 50, 50));
                previewSprite.m_Sprite.setPosition({ 320.0f, 320.0f });
                previewSprite.CenterOrigin();
                m_Viewport.draw(previewSprite.m_Sprite);
                m_Viewport.display();

                ImGui::Image(m_Viewport);
                ImGui::EndChild();
            }
            ImGui::Checkbox("Loop Animation", &m_Looping);
        }
        ImGui::End();

    }

    void AnimationTool::Open(AnimationData* data)
    {
        currentData = data;
        currentFrame = 0;
        elapsedTime = 0.0f;
        isFinished = false;
        m_isOpen = true;

        if (currentData)
        {
            previewSprite.m_Sprite.setTexture(ResourceManager::Get().GetResource<sf::Texture>(data->textureID));
            SpriteCords& sc = currentData->spriteCords[0];
            previewSprite.SetTextureRect(sf::IntRect({ sc.x, sc.y }, { sc.width, sc.height }));
            previewSprite.CenterOrigin();
        }
    }

    void AnimationTool::Animate(sf::Time tick)
    {
        // This block is pretty much completely copy/pasted from the renderer with
        // just a few unnecessary lines cut out 

        if(currentData != nullptr && !isFinished)
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
        delete this;
    }
}