#include "AudioComp.h"
#include "Core/ResourceManager/ResourceManager.h"

namespace Spoon
{
    void AudioComp::PlaySound() {
        if (sound.has_value())
            sound->play();
    }

    void AudioComp::StopSound() {
        if (sound.has_value())
            sound->stop();
    }

    void AudioComp::LoadSound(const std::string& fileId) {
        sound = sf::Sound(ResourceManager::Get().GetResource<sf::SoundBuffer>(fileId));
    }

    void AudioComp::OnReflect() {
        // listbox of available sound buffers in the resource manager
        ImGui::TextDisabled("Select the sound buffer to use for this audio component");
        if (ImGui::BeginCombo("Sound Buffer", bufferId.c_str()))
        {
            for (const auto& [id, resource] : ResourceManager::Get().GetSounds())
            {
                bool isSelected = (bufferId == id);
                if (ImGui::Selectable(id.c_str(), isSelected))
                    bufferId = id;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::TextDisabled("Sound Settings");
        ImGui::SliderFloat("Volume", &volume, 0.0f, 100.0f); ImGui::SameLine();
        HelpMarker("Adjust the volume of the sound (0 = silent, 100 = full volume)");
    }
}