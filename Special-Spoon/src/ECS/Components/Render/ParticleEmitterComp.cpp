#include "ParticleEmitterComp.h"
#include "Core/Application.h"
#include "Core/ResourceManager/ResourceManager.h"

namespace Spoon
{
    void ParticleEmitterComp::OnReflect()
    {
        static int setMaxParticles = 0;
        ImGui::TextDisabled("Emitter Settings");
        ImGui::SliderFloat("Emission Rate:", &emissionRate, 0.0f, 100.0f);
        ImGui::InputFloat("Emission Spread:", &emissionSpread);
        ImGui::SliderFloat2("Velocity Range:", &velocityRange.x, -100.0f, 100.0f);

        ImGui::TextDisabled("Particle Settings");
        ImGui::InputFloat("Particle Lifetime:", &particleLifetime);
        ImGui::SliderFloat2("Particle Size Range:", &particleSizeRange.x, 0.0f, 100.0f);
        
        ImGui::TextDisabled("Texture Settings");
        if (ImGui::BeginChild("Texture Explorer"))
        {
            for (const auto& [id, texture] : ResourceManager::Get().GetTextures())
            {
                if (ImGui::ImageButton(id.c_str(), texture, sf::Vector2f(64, 64)))
                {
                    textureID = id;
                }
            }

            if (ImGui::Button("Edit Texture Rect"))
            {
                editingRect = true;
            }

            if (editingRect)
            {
                sf::Texture& texture = ResourceManager::Get().GetResource<sf::Texture>(textureID);
                Application::Get().GetEditor().EditTextureRect(editingRect, texture, textureRect);
            }

            ImGui::Text("Texture ID: %s", textureID.c_str());
            ImGui::Text("Texture Rect: (%d, %d, %d, %d)", 
                textureRect.position.x, textureRect.position.y,
                textureRect.size.x, textureRect.size.y);
            ImGui::EndChild();
        }

        ImGui::TextDisabled("Lifecycle");
        ImGui::Checkbox("Is Active", &isActive);
        ImGui::Checkbox("Looping", &looping);
        ImGui::Text("Elapsed Time: %f", elapsedTime);
        ImGui::InputFloat("Total Lifetime:", &totalLifetime);
        ImGui::InputInt("Max Particles:", &setMaxParticles);
        maxParticles = static_cast<size_t>(setMaxParticles);

        ImGui::TextDisabled("Runtime Stats");
        ImGui::Text("Particle Pool Size: %zu", particlePool.size());
    }

    void ParticleEmitterComp::PreRender(EntityManager& manager, UUID id)
    {
        const auto& projection =
        Application::Get().GetRenderer().GetProjection();

        if (vertices.size() != particlePool.size() * 6)
            return;

        const sf::Vector2f halfSize{
            static_cast<float>(textureRect.size.x) * 0.5f,
            static_cast<float>(textureRect.size.y) * 0.5f
        };

        for (size_t i = 0; i < particlePool.size(); ++i)
        {
            const sf::Vector2f center =
                projection.LogicalToScreen(particlePool[i].position);

            const sf::Vector2f topLeft = center - halfSize;
            const sf::Vector2f topRight =
                center + sf::Vector2f{halfSize.x, -halfSize.y};
            const sf::Vector2f bottomRight = center + halfSize;
            const sf::Vector2f bottomLeft =
                center + sf::Vector2f{-halfSize.x, halfSize.y};

            const size_t base = i * 6;
            vertices[base + 0].position = topLeft;
            vertices[base + 1].position = topRight;
            vertices[base + 2].position = bottomRight;
            vertices[base + 3].position = topLeft;
            vertices[base + 4].position = bottomRight;
            vertices[base + 5].position = bottomLeft;
        }
    }

    void ParticleEmitterComp::Render(sf::RenderTarget& target, sf::RenderStates states)
    {
        states.texture = &ResourceManager::Get().GetResource<sf::Texture>(textureID);
        target.draw(vertices.data(), vertices.size(), sf::PrimitiveType::Triangles, states);
    }

    sf::Vector2f ParticleEmitterComp::GetPosition()
    {
        return sf::Vector2f({0.0f, 0.0f});
    }
}
