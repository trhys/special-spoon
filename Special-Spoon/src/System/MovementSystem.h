#pragma once

#include "System/System.h"
#include <vector>

namespace Spoon
{
    namespace MovementSystemUtil
    {
        uint32_t DirectionToStateID(const char* direction);
        bool IsMovementAction(const ActionType& action);
        bool IsMoving(const MovementComp& moveComp);
    }

	struct Action;

    class MovementSystem : public ISystem
    {
    public:
        MovementSystem() : ISystem::ISystem("Movement") {}
        ~MovementSystem() {}

        // scheduler methods
        std::vector<std::string> RunBefore() const override { return { "Physics", "Collision" }; }

        // returns a vector representing movement intent - modifiable downstream
        sf::Vector2f BuildIntent(const std::vector<const Action*>& actions,  const WorldProjection& projection);

        void Update(sf::Time tick, EntityManager& manager) override;

    private:
        std::unordered_set<UUID> m_ActionDrivenEntities;                // builds from action queue per frame
        std::unordered_map<UUID, sf::Vector2f> m_LastActionVelocity;    // state checker per entity
    };
}
