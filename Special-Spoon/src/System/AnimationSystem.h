#pragma once

#include "System.h"
#include "ECS/ECS.h"
#include "Core/EntityManager.h"

namespace Spoon
{
    class AnimationSystem : public ISystem
    {
    public: 
        AnimationSystem() : ISystem::ISystem("Animation") {}
        ~AnimationSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override;

    private:
        void FadeAnimation(sf::Time tick, FadeComp& comp);
        void BlinkAnimation(sf::Time tick, BlinkComp& comp);
    };
}