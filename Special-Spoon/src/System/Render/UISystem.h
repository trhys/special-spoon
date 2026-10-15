#pragma once

#include "System/System.h"
#include "UICursor.h"
#include "Core/ActionQueue.h"

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
            return { "Movement", "Animation" };
        }

        void SetCursor(EntityManager& manager);
        void Update(sf::Time tick, EntityManager& manager) override;
        void OnReflect() override;
    private:
        UICursor m_Cursor;
    };
}