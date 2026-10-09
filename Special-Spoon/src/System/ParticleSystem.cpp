#include "ParticleSystem.h"
#include "ECS/Components/Render/ParticleEmitterComp.h"
#include "ECS/Components/Render/SpriteComp.h"
#include "ECS/Components/TransformComp.h"
#include "Core/ResourceManager/ResourceManager.h"

#include <random>

namespace Spoon
{
    // random generator for particle properties such as size jitter and velocity
    std::random_device rd;
    std::mt19937 gen(rd());

    void ParticleSystem::Update(sf::Time tick, EntityManager& manager)
    {
        // get all emitters + transform
        auto& emitterArray = manager.GetArray<ParticleEmitterComp>(ParticleEmitterComp::Name);
        auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
        for (size_t index = 0; index < emitterArray.m_Components.size(); index++)
        {
            auto& emitter = emitterArray.m_Components[index];
            if (!emitter.isActive)
                continue;

            UUID id = emitterArray.m_IndexToId[index];
            auto& transform = transformArray.m_Components[transformArray.m_IdToIndex[id]];
            
            if (emitter.elapsedTime >= emitter.totalLifetime && !emitter.looping)
                emitter.isActive = false;
            emitter.elapsedTime += tick.asSeconds();

            UpdateEmitter(tick, manager, emitter, transform);
        }
    }

    void ParticleSystem::UpdateEmitter(sf::Time tick, EntityManager& manager, ParticleEmitterComp& emitter, TransformComp& transform)
    {
        // iterate emitter particle pool
        for (size_t i = 0; i < emitter.particlePool.size(); /* no increment here */)
        {
            auto& particle = emitter.particlePool[i];

            particle.remainingLifetime -= tick.asSeconds();

            // check lifetime
            if (particle.remainingLifetime <= 0.0f)
            {
                // swap and pop dead particle
                std::swap(particle, emitter.particlePool.back());
                emitter.particlePool.pop_back();
            } else {
                // update particle position based on its velocity
                particle.position += (particle.velocity * tick.asSeconds());

                // interpolate particle color and size based on its remaining lifetime
                float lifeRatio = particle.remainingLifetime / particle.maxLifetime;
                particle.color = InterpolateColor(emitter.particleStartColor, emitter.particleEndColor, 1.0f - lifeRatio);
                float startSize = emitter.particleStartSize * particle.sizeJitter;
                float endSize = emitter.particleEndSize * particle.sizeJitter;
                particle.size = InterpolateSize(startSize, endSize, 1.0f - lifeRatio);

                i++;
            }
        }

        // create entities at emission rate if queue is not empty
        emitter.accumulatedTime += tick.asSeconds();

        while (emitter.particlePool.size() < emitter.maxParticles && emitter.accumulatedTime >= 1.0f / emitter.emissionRate)
        {
            emitter.accumulatedTime -= 1.0f / emitter.emissionRate;

            Particle particle;
            particle.remainingLifetime = emitter.particleLifetime;
            particle.maxLifetime = emitter.particleLifetime;
            particle.position = transform.GetPosition();
            particle.color = emitter.particleStartColor;
            std::uniform_real_distribution<float> sizeDist(emitter.particleSizeRange.x, emitter.particleSizeRange.y);
            particle.sizeJitter = sizeDist(gen);
            particle.size = emitter.particleStartSize * particle.sizeJitter;

            // set velocity at a random angle from emission spread
            // and select speed from velocity range
            float angle = ((float)rand() / RAND_MAX - 0.5f) * emitter.emissionSpread;
            float speed = emitter.velocityRange.x + ((float)rand() / RAND_MAX) * (emitter.velocityRange.y - emitter.velocityRange.x);
            particle.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
            emitter.particlePool.push_back(particle);
        }

        RebuildEmitterVertexArray(emitter);
    }

    void ParticleSystem::RebuildEmitterVertexArray(ParticleEmitterComp& emitter)
    {
        // clear and reserve the vertex array for the current particle pool
        emitter.vertices.clear();
        emitter.vertices.reserve(emitter.particlePool.size() * 6);

        // get half size of texture
        sf::Vector2f texSize = sf::Vector2f(
            static_cast<float>(emitter.textureRect.size.x),
            static_cast<float>(emitter.textureRect.size.y)
        );
        sf::Vector2f baseSize = texSize * 0.5f;

        for (size_t i = 0; i < emitter.particlePool.size(); ++i)
        {
            auto& particle = emitter.particlePool[i];
            
            // compute this particles vertex size
            sf::Vector2f halfSize = baseSize * particle.size;
            
            // center quad on particle position
            sf::Vector2f topLeft = particle.position - halfSize;
            sf::Vector2f topRight = particle.position + sf::Vector2f(halfSize.x, -halfSize.y);
            sf::Vector2f bottomRight = particle.position + halfSize;
            sf::Vector2f bottomLeft = particle.position + sf::Vector2f(-halfSize.x, halfSize.y);

            // texture coordinates
            sf::Vector2f texPos(emitter.textureRect.position);
            sf::Vector2f texBottomRight = texPos + texSize;

            // color
            sf::Color color = particle.color;

            // push vertices
            emitter.vertices.push_back(sf::Vertex{topLeft, color, texPos});
            emitter.vertices.push_back(sf::Vertex{topRight, color, sf::Vector2f(texBottomRight.x, texPos.y)});
            emitter.vertices.push_back(sf::Vertex{bottomRight, color, texBottomRight});
            emitter.vertices.push_back(sf::Vertex{topLeft, color, texPos});
            emitter.vertices.push_back(sf::Vertex{bottomRight, color, texBottomRight});
            emitter.vertices.push_back(sf::Vertex{bottomLeft, color, sf::Vector2f(texPos.x, texBottomRight.y)});
        }
    }

    sf::Color ParticleSystem::InterpolateColor(const sf::Color& start, const sf::Color& end, float lifeRatio)
    {
        lifeRatio = std::clamp(lifeRatio, 0.0f, 1.0f);
        return sf::Color(
            static_cast<uint8_t>(start.r + (end.r - start.r) * lifeRatio),
            static_cast<uint8_t>(start.g + (end.g - start.g) * lifeRatio),
            static_cast<uint8_t>(start.b + (end.b - start.b) * lifeRatio),
            static_cast<uint8_t>(start.a + (end.a - start.a) * lifeRatio)
        );
    }

    float ParticleSystem::InterpolateSize(const float& start, const float& end, float lifeRatio)
    {
        lifeRatio = std::clamp(lifeRatio, 0.0f, 1.0f);
        return float(
            start + (end - start) * lifeRatio
        );
    }
}