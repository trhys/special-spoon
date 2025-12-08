#pragma once

#include "System.h"
#include "ECS/ECS.h"
#include "Core/EntityManager.h"

namespace Spoon
{
    class AnimationSystem : public ISystem
    {
    public: 
        AnimationSystem() {}
        ~AnimationSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override;

        void FadeAnimation(sf::Time tick, FadeComp& comp, ColorComp& colorcomp);
        void BlinkAnimation(sf::Time tick, BlinkComp& comp, ColorComp& colorcomp);

    private:
    };
}