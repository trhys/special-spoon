#pragma once

#include "ECS/Components/Component.h"

#include <SFML/Audio.hpp>
#include <optional>

namespace Spoon
{
    struct AudioComp : public ComponentBase<AudioComp> {
        AudioComp() : ComponentBase(Name) {}

        static constexpr const char* Name = "AudioComp";

        // sound settings
        std::string bufferId;   // id to buffer loaded in resource manager
        float volume = 100.0f;  // sound volume

        std::optional<sf::Sound> sound;

        void PlaySound();
        void StopSound();

        // load sound buffer from the resource manager
        void LoadSound(const std::string& fileId);

        void OnReflect() override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioComp, bufferId, volume)
}