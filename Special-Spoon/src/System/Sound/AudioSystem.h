#pragma once

#include "System/System.h"

namespace Spoon
{
    class AudioSystem : public ISystem {
    public:
        AudioSystem() : ISystem::ISystem("AudioSystem") {}

        // scheduler methods
        std::vector<std::string> RunBefore() const override {
            return {};
        }
        std::vector<std::string> RunAfter() const override {
            return {};
        }

        void Update(sf::Time tick, EntityManager& manager) override;
        void OnReflect() override;
    };
}