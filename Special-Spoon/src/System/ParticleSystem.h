#pragma once

#include "System.h"

namespace Spoon
{
    struct ParticleEmitterComp;
    struct TransformComp;

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
            void UpdateEmitter(sf::Time tick, EntityManager& manager, ParticleEmitterComp& emitter, TransformComp& transform);
            void RebuildEmitterVertexArray(ParticleEmitterComp& emitter);
    };
}