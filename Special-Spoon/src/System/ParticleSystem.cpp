#include "ParticleSystem.h"
#include "ECS/Components/Render/ParticleEmitterComp.h"
#include "ECS/Components/Render/SpriteComp.h"
#include "ECS/Components/TransformComp.h"
#include "Core/ResourceManager/ResourceManager.h"

namespace Spoon
{
    void ParticleSystem::Update(sf::Time tick, EntityManager& manager)
    {
        // get all emitters
        auto& emitterArray = manager.GetArray<ParticleEmitterComp>(ParticleEmitterComp::Name);
        for (size_t index = 0; index < emitterArray.m_Components.size(); index++)
        {
            auto& emitter = emitterArray.m_Components[index];
            UUID id = emitterArray.m_IndexToId[index];
            if (!emitter.IsInitialized())
                InitializeEmitter(manager, emitter);
            if (!emitter.isActive)
                continue;
            if (emitter.elapsedTime >= emitter.totalLifetime && !emitter.looping)
                emitter.isActive = false;
            emitter.elapsedTime += tick.asSeconds();

            UpdateEmitter(tick, manager, emitter);
        }
    }

    void ParticleSystem::InitializeEmitter(EntityManager& manager, ParticleEmitterComp& emitter)
    {
        for (size_t i = 0; i < emitter.maxParticles; i++)
        {
            UUID id = manager.GenerateID();
            manager.MarkEntityAsRuntimeOnly(id);

            // add comps
            manager.MakeComponent<SpriteComp>(id, SpriteComp::Name,
                ResourceManager::Get().GetResource<sf::Texture>(emitter.textureID),
                emitter.textureRect, false, emitter.textureID);
            manager.MakeComponent<TransformComp>(id, TransformComp::Name);
            manager.MakeComponent<ParticleComp>(id, ParticleComp::Name);

            emitter.particlePool.push(id);
        }
    }

    void ParticleSystem::UpdateEmitter(sf::Time tick, EntityManager& manager, ParticleEmitterComp& emitter)
    {
        // get active particles and cull the dead ones
        for (size_t i = 0; i < emitter.activeParticles.size(); i++)
        {
            // get the entity
            UUID particleID = emitter.activeParticles[i];
            auto& particle = manager.GetComponent<ParticleComp>(particleID, ParticleComp::Name);
            auto& sprite = manager.GetComponent<SpriteComp>(particleID, SpriteComp::Name);
            auto& transform = manager.GetComponent<TransformComp>(particleID, TransformComp::Name);
            
            // check lifetime
            if (particle.remainingLifetime <= 0.0f)
            {
                sprite.SetActive(false);
                emitter.activeParticles.erase(emitter.activeParticles.begin() + i);
                emitter.particlePool.push(particleID);
                i--;
            }
            else
            {
                particle.remainingLifetime -= tick.asSeconds();
            }

            // update the transform with the particle's current state
            transform.Move(particle.velocity * tick.asSeconds());
        }

        // create entities at emission rate if queue is not empty
        emitter.accumulatedTime += tick.asSeconds();

        while (!emitter.particlePool.empty() && emitter.accumulatedTime >= 1.0f / emitter.emissionRate)
        {
            emitter.accumulatedTime -= 1.0f / emitter.emissionRate;

            UUID id = emitter.particlePool.front();
            emitter.particlePool.pop();
            emitter.activeParticles.push_back(id);

            auto& particle = manager.GetComponent<ParticleComp>(id, ParticleComp::Name);
            auto& sprite = manager.GetComponent<SpriteComp>(id, SpriteComp::Name);
            auto& transform = manager.GetComponent<TransformComp>(id, TransformComp::Name);

            particle.remainingLifetime = particle.maxLifetime;
            sprite.SetActive(true);
            // TODO: scale, color, etc

            // set velocity at a random angle from emission spread
            // and select speed from velocity range
            float angle = ((float)rand() / RAND_MAX - 0.5f) * emitter.emissionSpread;
            float speed = emitter.velocityRange.x + ((float)rand() / RAND_MAX) * (emitter.velocityRange.y - emitter.velocityRange.x);
            particle.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        }
    }
}