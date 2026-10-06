#pragma once

#include "ECS/Components/Component.h"
#include "Core/Renderer/Renderable.h"

#include <string>
#include <queue>
#include <vector>

namespace Spoon
{
    struct ParticleEmitterComp : public ComponentBase<ParticleEmitterComp> {
        ParticleEmitterComp() : ComponentBase::ComponentBase(Name) {}
        static constexpr const char* Name = "ParticleEmitter";

        // emittor settings
        sf::Vector2f emissionRate;
        float emissionSpread;
        sf::Vector2f velocityRange;

        // particle settings
        float particleLifetime;
        sf::Vector2f particleSizeRange;
        std::string textureID;
        sf::IntRect textureRect;

        // lifecycle
        bool isActive;
        bool looping;
        float elapsedTime;
        float totalLifetime;

        // pooling - runtime state only
        std::queue<UUID> particlePool;
        std::vector<UUID> activeParticles;
        size_t maxParticles;

        // helper for initializing the particle emitter
        bool IsInitialized() const { return !particlePool.empty(); }

        // editor interface
        void OnReflect() override;
        bool editingRect = false;
    };

    struct ParticleComp : public ComponentBase<ParticleComp>, public IRenderable {
        ParticleComp() : ComponentBase::ComponentBase(Name) {}
        static constexpr const char* Name = "Particle";

        float remainingLifetime;
        float maxLifetime;
        UUID parentEmitter;
    };
}

// serialization
namespace Spoon
{
    inline void to_json(json& j, const ParticleEmitterComp& comp)
    {
        j = json{
            {"emissionRate", comp.emissionRate},
            {"emissionSpread", comp.emissionSpread},
            {"velocityRange", comp.velocityRange},
            {"particleLifetime", comp.particleLifetime},
            {"particleSizeRange", comp.particleSizeRange},
            {"textureID", comp.textureID},
            {"textureRect", comp.textureRect},
            {"isActive", comp.isActive},
            {"looping", comp.looping},
            {"elapsedTime", comp.elapsedTime},
            {"totalLifetime", comp.totalLifetime},
            {"maxParticles", comp.maxParticles}
        };
    }

    inline void from_json(const json& j, ParticleEmitterComp& comp)
    {
        j.at("emissionRate").get_to(comp.emissionRate);
        j.at("emissionSpread").get_to(comp.emissionSpread);
        j.at("velocityRange").get_to(comp.velocityRange);
        j.at("particleLifetime").get_to(comp.particleLifetime);
        j.at("particleSizeRange").get_to(comp.particleSizeRange);
        j.at("textureID").get_to(comp.textureID);
        j.at("textureRect").get_to(comp.textureRect);
        j.at("isActive").get_to(comp.isActive);
        j.at("looping").get_to(comp.looping);
        j.at("elapsedTime").get_to(comp.elapsedTime);
        j.at("totalLifetime").get_to(comp.totalLifetime);
        j.at("maxParticles").get_to(comp.maxParticles);
    }

    inline void to_json(json& j, const ParticleComp& comp) {}
    inline void from_json(const json& j, ParticleComp& comp) {}
}