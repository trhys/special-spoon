#pragma once

#include "System/System.h"

namespace Spoon
{
    class UISystem : public ISystem
    {
    public:
        UISystem() : ISystem(Name) {}

        static constexpr const char* Name = "UI";

        // scheduler methods
        std::vector<std::string> RunAfter() const override
        {
            return {};
        }
        std::vector<std::string> RunBefore() const override
        {
            return {};
        }

        void Update(sf::Time tick, EntityManager& manager) override;
        void OnReflect() override;
    };
}