#include "AnimationSystem.h"

namespace Spoon
{
    void AnimationSystem::FadeAnimation(sf::Time tick, FadeComp& fadecomp, ColorComp& colorcomp)
    {
        fadecomp.m_FadeTimer = fadecomp.m_FadeTimer + tick;
        float progress = std::min(fadecomp.m_FadeTimer.asSeconds() / fadecomp.m_FadeRate, 1.0f);

        if(fadecomp.isFadingIn)
        {
            colorcomp.SetAlpha(static_cast<std::uint8_t>(0.0f + (255.0f - 0.0f) * progress));
        }
        else if(fadecomp.isFadingOut)
        {
            colorcomp.SetAlpha(static_cast<std::uint8_t>(255.0f + (0.0f - 255.0f) * progress));
        }

        if(progress >= 1.0f) { fadecomp.ToggleActive(); }
    }

    void AnimationSystem::BlinkAnimation(sf::Time tick, BlinkComp& blinkcomp, ColorComp& colorcomp)
    {
        blinkcomp.m_BlinkTimer = blinkcomp.m_BlinkTimer + tick;
        float progress = std::min(blinkcomp.m_BlinkTimer.asSeconds() / blinkcomp.m_Blinkrate, 1.0f);

        if(!blinkcomp.isGoingUp)
        {
            colorcomp.SetAlpha(static_cast<std::uint8_t>(255.0f + (0.0f - 255.0f) * progress));
        }
        else
        {
            colorcomp.SetAlpha(static_cast<std::uint8_t>(0.0f + (255.0f - 0.0f) * progress));
        }

        if(progress >= 1.0f)
        {
            blinkcomp.ToggleSwitch();
            blinkcomp.m_BlinkTimer.Zero;
        }
    }

    void AnimationSystem::Update(sf::Time tick, EntityManager& manager)
    {
        // Default animation components
        auto& fadeArray = manager.GetArray<FadeComp>();
        auto& blinkArray = manager.GetArray<BlinkComp>();
        auto& colorArray = manager.GetArray<ColorComp>();

        for(size_t in = 0; in < fadeArray.m_Components.size(); in++)
        {
            FadeComp& fadecomp = fadeArray.m_Components[in];
            UUID id = fadeArray.m_IndexToId[in];

            if(fadecomp.isActive)
            {
                if(colorArray.m_IdToIndex.count(id))
                {
                    ColorComp& colorcomp = colorArray.m_Components[colorArray.m_IdToIndex[id]];
                    FadeAnimation(tick, fadecomp, colorcomp);
                }
            }
        }

        for(size_t in = 0; in < blinkArray.m_Components.size(); in++)
        {
            BlinkComp& blinkcomp = blinkArray.m_Components[in];
            UUID id = blinkArray.m_IndexToId[in];

            if(blinkcomp.isActive)
            {
                if(colorArray.m_IdToIndex.count(id))
                {
                    ColorComp& colorcomp = colorArray.m_Components[colorArray.m_IdToIndex[id]];
                    BlinkAnimation(tick, blinkcomp, colorcomp);
                }
            }
        }

        // AnimationComp based animations (defined by scene_data.json files)
        auto& animationArray = manager.GetArray<AnimationComp>();
        auto& statusArray = manager.GetArray<StatusComp>();
        auto& spriteArray = manager.GetArray<SpriteComp>();

        for(size_t in = 0; in < animationArray.m_Components.size(); in++)
        {
            AnimationComp& animComp = animationArray.m_Components[in];
            UUID id = animationArray.m_IndexToId[in];

            // Update animation based on StatusComp current state
            if(statusArray.m_IdToIndex.count(id))
            {
                StatusComp& statusComp = statusArray.m_Components[statusArray.m_IdToIndex[id]];
                if(animComp.m_AnimationData->ID != animComp.m_AnimationMap[statusComp.m_CurrentState])
                {
                    std::string newAnimID = animComp.m_AnimationMap[statusComp.m_CurrentState];
                    animComp.SetAnimationData(newAnimID);
                    animComp.currentFrame = 0;
                    animComp.elapsedTime = 0.0f;
                    animComp.isFinished = false;
                }
            }
            else
            {
                throw std::runtime_error("Entity is missing StatusComp required for AnimationComp state management.");
            }
            if(animComp.m_AnimationData != nullptr && !animComp.isFinished)
            {
                animComp.elapsedTime += tick.asSeconds();
                while(animComp.elapsedTime >= animComp.m_AnimationData->frameRate)
                {
                    animComp.elapsedTime -= animComp.m_AnimationData->frameRate;
                    animComp.currentFrame++;
                }
                if(animComp.currentFrame >= static_cast<int>(animComp.m_AnimationData->spriteCords.size()))
                {
                    if(animComp.m_AnimationData->isLooping)
                    {
                        animComp.currentFrame = 0;
                    }
                    else
                    {
                        animComp.currentFrame = static_cast<int>(animComp.m_AnimationData->spriteCords.size()) - 1;
                        animComp.isFinished = true;
                    }
                }
                if(spriteArray.m_IdToIndex.count(id))
                {
                    SpriteComp& spriteComp = spriteArray.m_Components[spriteArray.m_IdToIndex[id]];
                    SpriteCords& sc = animComp.m_AnimationData->spriteCords[animComp.currentFrame];
                    spriteComp.SetTextureRect(sf::IntRect({sc.x, sc.y}, {sc.width, sc.height}));
                }
            }
            else if(animComp.m_AnimationData != nullptr && animComp.isFinished)
            {
                // Animation is finished, do nothing
            }   
            else
            {
                throw std::runtime_error("AnimationData is null for Entity");
            }
        }
    }
}