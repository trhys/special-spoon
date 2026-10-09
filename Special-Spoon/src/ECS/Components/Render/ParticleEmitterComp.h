#pragma once

#include "ECS/Components/Component.h"
#include "Core/Renderer/Renderable.h"

#include <string>
#include <queue>
#include <vector>

namespace Spoon
{
    struct Particle {
        float remainingLifetime;
        float maxLifetime;
        sf::Vector2f velocity;
        sf::Vector2f position;
    };
    
    struct ParticleEmitterComp : public ComponentBase<ParticleEmitterComp>, public IRenderable {
        ParticleEmitterComp() : ComponentBase::ComponentBase(Name) {
            // initialize particle pool memory
            particlePool.reserve(maxParticles);
        }

        static constexpr const char* Name = "ParticleEmitter";

        // emittor settings
        float emissionRate;
        float emissionSpread;
        sf::Vector2f velocityRange;
        size_t maxParticles = 1000;

        // particle settings
        float particleLifetime;
        sf::Vector2f particleSizeRange;
        std::string textureID = "empty";
        sf::IntRect textureRect;

        // lifecycle
        bool isActive;
        bool looping;
        float elapsedTime = 0.0f;
        float totalLifetime;
        float accumulatedTime = 0.0f;
        
        // pooling - runtime state only
        std::vector<Particle> particlePool;
        std::vector<sf::Vertex> vertices;

        // editor interface
        void OnReflect() override;
        bool editingRect = false;

        // renderable interface
        void PreRender(EntityManager& manager, UUID id) override;
        void Render(sf::RenderTarget& target, sf::RenderStates states) override;
        sf::Vector2f GetPosition() override;
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
}
