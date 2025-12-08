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

    }
}
