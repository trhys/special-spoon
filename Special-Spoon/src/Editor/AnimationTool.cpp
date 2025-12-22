#include "AnimationTool.h"

namespace Spoon
{
    void AnimationTool::Update(sf::Time tick)
    {
        ImGui::Begin("Animation Tool", &m_isOpen, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            // todo
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

        if( ImGui::BeginChild("AT Viewport", ImVec2(640, 640), ImGuiChildFlags_Borders))
        {
            if(m_Playback) Animate(tick);

            m_Viewport.clear(sf::Color(50, 50, 50));
            previewSprite.m_Sprite.setPosition({ 320.0f, 320.0f });
            previewSprite.CenterOrigin();
            m_Viewport.draw(previewSprite.m_Sprite);
            m_Viewport.display();

            ImGui::Image(m_Viewport);
            ImGui::EndChild();
        }
        ImGui::Checkbox("Loop Animation", &m_Looping);
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
}