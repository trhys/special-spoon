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

        // action -> sound mappings
        std::unordered_map<uint32_t, std::string> actionBuffers;

        std::optional<sf::Sound> sound;

        void PlaySound();
        void StopSound();

        // load sound buffer from the resource manager
        void LoadSound(const std::string& fileId);

        // check if action is mapped
        bool IsActionMapped(uint32_t id);

        void OnReflect() override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioComp, actionBuffers, bufferId, volume)
}
