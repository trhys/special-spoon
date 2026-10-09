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
        float sizeJitter;
        sf::Color color;
        float size;
    };
    
    struct ParticleEmitterComp : public ComponentBase<ParticleEmitterComp>, public IRenderable {
        ParticleEmitterComp() : ComponentBase::ComponentBase(Name) {
            // initialize particle pool memory
            particlePool.reserve(maxParticles);
        }

        static constexpr const char* Name = "ParticleEmitter";

        // emittor settings
        float emissionRate = 1000.0f;                           // particles emitted per second
        float emissionSpread = 6.28f;                           // spread angle in radians
        sf::Vector2f velocityRange = {0.0f, 100.0f};            // min and max velocity for particles
        size_t maxParticles = 1000;                             // maximum number of particles in the pool

        // particle settings
        float particleLifetime = 1.0f;                      // particle lifecycle in seconds
        float particleVelocityDamping = 0.98f;              // how much the particle velocity is reduced each frame
        sf::Vector2f particleSizeRange = {0.0f, 2.0f};      // scalar modification - a range
        float particleStartSize = 1.0f;                     // selected from above
        float particleEndSize = 1.0f;                       // ""
        sf::Vector2f particleSpawnOffset = {0.0f, 0.0f};    // offset from the emitter's position where particles spawn
        sf::Color particleStartColor = sf::Color::White;    // interpolation settings for color
        sf::Color particleEndColor = sf::Color::White;      // ""

        std::string textureID = "empty";
        sf::IntRect textureRect;

        // lifecycle
        bool isActive = true;
        bool looping = true;
        float elapsedTime = 0.0f;
        float totalLifetime = 1.0f;
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

        // preset save/load
        void SavePreset(const std::string& presetName);
        void LoadPreset(const std::string& presetName);
        void LoadFromPreset(const json& j);
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
            {"particleVelocityDamping", comp.particleVelocityDamping},
            {"particleLifetime", comp.particleLifetime},
            {"particleSizeRange", comp.particleSizeRange},
            {"particleSpawnOffset", comp.particleSpawnOffset},
            {"textureID", comp.textureID},
            {"textureRect", comp.textureRect},
            {"isActive", comp.isActive},
            {"looping", comp.looping},
            {"totalLifetime", comp.totalLifetime},
            {"particleStartColor", comp.particleStartColor},
            {"particleEndColor", comp.particleEndColor},
            {"particleStartSize", comp.particleStartSize},
            {"particleEndSize", comp.particleEndSize},
            {"maxParticles", comp.maxParticles}
        };
    }

    inline void from_json(const json& j, ParticleEmitterComp& comp)
    {
        j.at("emissionRate").get_to(comp.emissionRate);
        j.at("emissionSpread").get_to(comp.emissionSpread);
        j.at("velocityRange").get_to(comp.velocityRange);
        j.at("particleVelocityDamping").get_to(comp.particleVelocityDamping);
        j.at("particleLifetime").get_to(comp.particleLifetime);
        j.at("particleSizeRange").get_to(comp.particleSizeRange);
        j.at("particleSpawnOffset").get_to(comp.particleSpawnOffset);
        j.at("textureID").get_to(comp.textureID);
        j.at("textureRect").get_to(comp.textureRect);
        j.at("isActive").get_to(comp.isActive);
        j.at("looping").get_to(comp.looping);
        j.at("totalLifetime").get_to(comp.totalLifetime);
        j.at("particleStartColor").get_to(comp.particleStartColor);
        j.at("particleEndColor").get_to(comp.particleEndColor);
        j.at("particleStartSize").get_to(comp.particleStartSize);
        j.at("particleEndSize").get_to(comp.particleEndSize);
        j.at("maxParticles").get_to(comp.maxParticles);
    }
}
