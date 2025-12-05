#pragma once

#include "ECS/ECS.h"
#include "Scene/Scene.h"

namespace Spoon
{
    class AnimationSystem
    {
    public: 
        AnimationSystem() {}
        ~AnimationSystem() {}

        void Run(sf::Time tick, Scene& scene);

        void FadeAnimation(sf::Time tick, FadeComp& comp, ColorComp& colorcomp);
        void BlinkAnimation(sf::Time tick, BlinkComp& comp, ColorComp& colorcomp);

    private:
    };
}