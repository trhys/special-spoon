#pragma once

#include "System.h"

namespace Spoon
{
    struct ParticleEmitterComp;

    class ParticleSystem : public ISystem {
        public:
            ParticleSystem() : ISystem("Particles") {}

            // scheduler methods
            std::vector<std::string> RunAfter() const override 
            { return { "Movement", "Physics"}; }
            std::vector<std::string> RunBefore() const override 
            { return {"Collision", "Animation"}; }

            void Update(sf::Time tick, EntityManager& manager) override;
            void OnReflect() override {}

        private:
            void InitializeEmitter(EntityManager& manager, ParticleEmitterComp& emitter);
            void UpdateEmitter(sf::Time tick, EntityManager& manager, ParticleEmitterComp& emitter);

    };
}