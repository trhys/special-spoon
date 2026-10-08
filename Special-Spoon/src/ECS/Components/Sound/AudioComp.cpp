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
        
    }
}