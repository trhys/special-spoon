#include "ParticleEmitterComp.h"
#include "Core/Application.h"
#include "Core/ResourceManager/ResourceManager.h"

namespace Spoon
{
    // helper for the color edits
    auto editColor = [](const char* label, sf::Color& color)
    {
        float rgba[4]{
            color.r / 255.0f,
            color.g / 255.0f,
            color.b / 255.0f,
            color.a / 255.0f
        };

        if (ImGui::ColorEdit4(label, rgba))
        {
            auto toByte = [](float value) -> std::uint8_t
            {
                return static_cast<std::uint8_t>(
                    std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
            };

            color = sf::Color{
                toByte(rgba[0]),
                toByte(rgba[1]),
                toByte(rgba[2]),
                toByte(rgba[3])
            };
        }
    };

    void ParticleEmitterComp::OnReflect()
    {
        static int setMaxParticles = static_cast<int>(maxParticles);
        static bool setMaxParticlesChanged = false;

        // popup info for prompting preset save
        const char* presetSavePopupLabel = "Name New Preset";
        const char* presetOverwritePopupLabel = "Overwrite Existing Preset?";
        static char presetSaveInputBuffer[32] = "";
        const char* presetSelectPopupLabel = "Select Preset";

        ImGui::TextDisabled("Presets");
        if (ImGui::Button("Load Preset"))
        {
            ImGui::OpenPopup(presetSelectPopupLabel);
        }
        if (ImGui::BeginPopupModal(presetSelectPopupLabel, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginListBox("Available Presets"))
            {
                const auto& presets = Application::Get().GetProjectManager().GetAvailablePresets("particles");
                for (const auto& preset : presets)
                {
                    if (ImGui::Selectable(preset.c_str()))
                    {
                        LoadPreset(preset.c_str());
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Preset"))
        {
            auto path = Application::Get().GetProjectManager().GetCurrentProject()->presetsPath / "particles" / (std::string(presetSaveInputBuffer) + ".json");
            if (std::filesystem::exists(path))
                ImGui::OpenPopup(presetOverwritePopupLabel);
            else
                ImGui::OpenPopup(presetSavePopupLabel);
        }
        if (ImGui::BeginPopupModal(presetSavePopupLabel, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("Preset Name", presetSaveInputBuffer, IM_ARRAYSIZE(presetSaveInputBuffer));
            if (ImGui::Button("Save"))
            {
                SavePreset(presetSaveInputBuffer);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal(presetOverwritePopupLabel, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("A preset with this name already exists. Overwrite?");
            if (ImGui::Button("Yes"))
            {
                SavePreset(presetSaveInputBuffer);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();

        ImGui::TextDisabled("Emitter Settings");
        ImGui::SliderFloat("Emission Rate:", &emissionRate, 0.0f, 1000.0f); ImGui::SameLine();
        HelpMarker("Controls the rate at which particles are emitted. Unit is particles/second.");
        ImGui::InputFloat("Emission Spread:", &emissionSpread); ImGui::SameLine();
        HelpMarker("Controls the spread angle of emitted particles in radians.");
        ImGui::SliderFloat2("Velocity Range:", &velocityRange.x, -100.0f, 100.0f); ImGui::SameLine();
        HelpMarker("Controls the range of initial velocities for emitted particles.");

        ImGui::TextDisabled("Particle Settings");
        ImGui::InputFloat("Particle Lifetime:", &particleLifetime); ImGui::SameLine();
        HelpMarker("Controls the lifetime of each emitted particle in seconds.");
        ImGui::SliderFloat2("Particle Size Range:", &particleSizeRange.x, 0.0f, 100.0f); ImGui::SameLine();
        HelpMarker("Controls the range of size jitter for emitted particles. A scalar modifier of the particle's base size.");
        ImGui::SliderFloat2("Particle Spawn Offset:", &particleSpawnOffset.x, -100.0f, 100.0f); ImGui::SameLine();
        HelpMarker("Controls the offset from the emitter's position where particles are spawned.");
        ImGui::InputFloat("Particle Velocity Damping:", &particleVelocityDamping); ImGui::SameLine();
        HelpMarker("Controls the damping factor applied to particle velocities each frame. Value should be between 0.0 and 1.0.\n 0.0 applies instant stop. 1.0 applies no change.");
        
        ImGui::TextDisabled("Particle Interpolation Settings");
        editColor("Particle Start Color:", particleStartColor); ImGui::SameLine();
        HelpMarker("Controls the starting color of each emitted particle.");
        editColor("Particle End Color:", particleEndColor); ImGui::SameLine();
        HelpMarker("Controls the ending color of each emitted particle.");
        ImGui::InputFloat("Particle Start Size:", &particleStartSize); ImGui::SameLine();
        HelpMarker("Controls the starting size of each emitted particle.");
        ImGui::InputFloat("Particle End Size:", &particleEndSize); ImGui::SameLine();
        HelpMarker("Controls the ending size of each emitted particle.");

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

        // have to put this bool so the static var doesn't overwrite the actual maxParticles value  
        setMaxParticlesChanged |= ImGui::InputInt("Max Particles:", &setMaxParticles);
        if (setMaxParticlesChanged)
        {
            maxParticles = static_cast<size_t>(setMaxParticles);
            setMaxParticlesChanged = false;
        }

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

            // Calculate the positions of the particle's vertices based on its center and half size.
            // Apply offset to manually adjust the particle's position based on the spawn offset.
            const sf::Vector2f topLeft = center - halfSize + particleSpawnOffset;
            const sf::Vector2f topRight =
                center + sf::Vector2f{halfSize.x, -halfSize.y} + particleSpawnOffset;
            const sf::Vector2f bottomRight = center + halfSize + particleSpawnOffset;
            const sf::Vector2f bottomLeft =
                center + sf::Vector2f{-halfSize.x, halfSize.y} + particleSpawnOffset;

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

    void ParticleEmitterComp::SavePreset(const std::string& presetName)
    {
        std::filesystem::path presetsPath = Application::Get().GetProjectManager().GetCurrentProject()->presetsPath;
        std::filesystem::create_directories(presetsPath / "particles");
        const std::filesystem::path presetFile = presetsPath / "particles" / (presetName + ".json");
        
        json j = this->Serialize();
        std::ofstream fileStream(presetFile, std::ios::out | std::ios::trunc);
        if (fileStream.is_open())
        {
            fileStream << j.dump(4);
            fileStream.close();
        }
    }

    void ParticleEmitterComp::LoadPreset(const std::string& presetName)
    {
        const std::filesystem::path presetFile = Application::Get().GetProjectManager().GetCurrentProject()->presetsPath / "particles" / (presetName + ".json");
        if (std::filesystem::exists(presetFile))
        {
            std::ifstream fileStream(presetFile, std::ios::in);
            if (fileStream.is_open())
            {
                json j;
                fileStream >> j;
                fileStream.close();
                
                // reconstruct preset
                LoadFromPreset(j);
            }
        } else {
            SS_DEBUG_LOG("[ParticleEmitterComp] Preset file not found: " + presetFile.string());
            throw std::runtime_error("Preset file not found: " + presetFile.string());
        }
    }

    void ParticleEmitterComp::LoadFromPreset(const json& j)
    {
        from_json(j, *this);
    }
}
