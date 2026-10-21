#include "MovementSystem.h"
#include "Core/Application.h"

#include <cmath>
#include <Core/ActionQueue.h>

namespace Spoon
{
	namespace MovementSystemUtil
    {
        // Maps the 8-way facing direction computed by MovementSystem onto the
        // matching built-in movement state so StatusComp::m_CurrentState can
        // drive animation selection (see AnimationSystem).
        uint32_t DirectionToStateID(const char* direction)
        {
            if (std::strcmp(direction, "Up") == 0)        return BuiltInStates::MoveUp;
            if (std::strcmp(direction, "Down") == 0)      return BuiltInStates::MoveDown;
            if (std::strcmp(direction, "Left") == 0)      return BuiltInStates::MoveLeft;
            if (std::strcmp(direction, "Right") == 0)     return BuiltInStates::MoveRight;
            if (std::strcmp(direction, "UpRight") == 0)   return BuiltInStates::MoveUR;
            if (std::strcmp(direction, "UpLeft") == 0)    return BuiltInStates::MoveUL;
            if (std::strcmp(direction, "DownRight") == 0) return BuiltInStates::MoveDR;
            if (std::strcmp(direction, "DownLeft") == 0)  return BuiltInStates::MoveDL;

            return BuiltInStates::Idle;
        }

        bool IsMovementAction(const ActionType& action)
        {
            return action.m_ID == BuiltInActions::MoveLeft ||
                action.m_ID == BuiltInActions::MoveRight ||
                action.m_ID == BuiltInActions::MoveUp ||
                action.m_ID == BuiltInActions::MoveDown;
        }

        bool IsMoving(const MovementComp& moveComp)
        {
            constexpr float epsilon = 0.0001f;
            return std::abs(moveComp.m_Velocity.x) > epsilon ||
                std::abs(moveComp.m_Velocity.y) > epsilon;
        }
    }
    
    sf::Vector2f MovementSystem::BuildIntent(const std::vector<const Action*>& actions, const WorldProjection& projection)
    {
        bool moveLeft = false;
        bool moveRight = false;
        bool moveUp = false;
        bool moveDown = false;

        for (const Action* action : actions)
        {
            switch (action->m_ActionType.m_ID)
            {
            case BuiltInActions::MoveLeft:
                moveLeft = true;
                break;
            case BuiltInActions::MoveRight:
                moveRight = true;
                break;
            case BuiltInActions::MoveUp:
                moveUp = true;
                break;
            case BuiltInActions::MoveDown:
                moveDown = true;
                break;
            default:
                break;
            }
        }

        sf::Vector2f intent(
            (moveRight ? 1.0f : 0.0f) - (moveLeft ? 1.0f : 0.0f),
            (moveDown ? 1.0f : 0.0f) - (moveUp ? 1.0f : 0.0f)
        );

        // project intent from screen space to logical space
        // before normalizing diagonal movement
        // this ensures that diagonal movement is correctly scaled in logical space
        intent = projection.ScreenToLogical(intent);

        float magnitude = std::sqrt(intent.x * intent.x + intent.y * intent.y);
        if (magnitude > 0.0f)
        {
            intent /= magnitude;
        }

        return intent;
    }

    void MovementSystem::Update(sf::Time tick, EntityManager& manager)
    {
        auto& queue = Application::Get().GetActionQueue();
		auto& projection = Application::Get().GetRenderer().GetProjection();
        auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
        auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
        auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);
        auto& statusArray = manager.GetArray<StatusComp>(StatusComp::Name);
        std::unordered_map<UUID, std::vector<const Action*>> movementActions;

        for (const auto& action : queue.m_Queue)
        {
            if (MovementSystemUtil::IsMovementAction(action.m_ActionType))
            {
                movementActions[action.m_EntityID].push_back(&action);
            }
        }

        for (size_t index = 0; index < movementArray.m_Components.size(); index++)
        {
            MovementComp& moveComp = movementArray.m_Components[index];
            UUID ID = movementArray.m_IndexToId[index];
            const bool hasPhysics = physicsArray.m_IdToIndex.count(ID) > 0;

            moveComp.m_FrameIntent = { 0.0f, 0.0f };
            moveComp.m_ProposedDelta = { 0.0f, 0.0f };
            moveComp.m_WasCorrectedByPhysics = false;

            auto actionRange = movementActions.find(ID);
            if (actionRange != movementActions.end())
            {
                moveComp.m_FrameIntent = BuildIntent(actionRange->second, projection);
                moveComp.m_Velocity = moveComp.m_FrameIntent * moveComp.m_Speed;
                m_ActionDrivenEntities.insert(ID);
                m_LastActionVelocity[ID] = moveComp.m_Velocity;
            }
            else if (m_ActionDrivenEntities.erase(ID) > 0)
            {
                auto lastVelocity = m_LastActionVelocity.find(ID);
                bool velocityMatchesLastAction =
                    lastVelocity != m_LastActionVelocity.end() &&
                    std::abs(moveComp.m_Velocity.x - lastVelocity->second.x) < 0.0001f &&
                    std::abs(moveComp.m_Velocity.y - lastVelocity->second.y) < 0.0001f;

                if (velocityMatchesLastAction)
                {
                    moveComp.m_FrameIntent = { 0.0f, 0.0f };
                    bool preservePhysicsVelocity = false;
                    if (hasPhysics)
                    {
                        auto& physicsComp = manager.GetComponent<PhysicsComp>(ID, PhysicsComp::Name);
                        preservePhysicsVelocity = physicsComp.bodyType == BodyType::Dynamic;
                    }

                    if (!preservePhysicsVelocity)
                    {
                        moveComp.m_Velocity = { 0.0f, 0.0f };
                    }
                }
                else if (!hasPhysics && (moveComp.m_Velocity.x != 0.0f || moveComp.m_Velocity.y != 0.0f))
                {
                    float magnitude = std::sqrt(moveComp.m_Velocity.x * moveComp.m_Velocity.x + moveComp.m_Velocity.y * moveComp.m_Velocity.y);
                    if (magnitude > 0.0f)
                    {
                        moveComp.m_FrameIntent = moveComp.m_Velocity / magnitude;
                    }
                }

                m_LastActionVelocity.erase(ID);
            }
            else if (!hasPhysics && (moveComp.m_Velocity.x != 0.0f || moveComp.m_Velocity.y != 0.0f))
            {
                float magnitude = std::sqrt(moveComp.m_Velocity.x * moveComp.m_Velocity.x + moveComp.m_Velocity.y * moveComp.m_Velocity.y);
                if (magnitude > 0.0f)
                {
                    moveComp.m_FrameIntent = moveComp.m_Velocity / magnitude;
                }
            }

            if (transformArray.m_IdToIndex.count(ID))
            {
                TransformComp& transComp = manager.GetComponent<TransformComp>(ID, TransformComp::Name);
                auto& statusComp = manager.GetComponent<StatusComp>(ID, StatusComp::Name);
                moveComp.m_ProposedDelta = moveComp.m_Velocity * tick.asSeconds();
                if (!hasPhysics)
                {
                    transComp.Move(moveComp.m_ProposedDelta);
                }

                // Determine direction of travel
                sf::Vector2f facingVector = moveComp.m_FrameIntent;
                if (facingVector.x == 0.0f && facingVector.y == 0.0f)
                {
                    facingVector = moveComp.m_Velocity;
                }

                if (facingVector.x != 0.0f || facingVector.y != 0.0f)
                {
                    facingVector = projection.LogicalToScreen(facingVector);
                    const float angle =
                        std::atan2(facingVector.y, facingVector.x) *
                        180.f / 3.14159265f;
                    transComp.m_Facing.m_Angle =
                        std::fmod(angle + 360.f, 360.f);
                    transComp.m_Facing.UpdateDirection();

                    // set status from orientation
                    statusComp.ApplyState(StateType{
                        MovementSystemUtil::DirectionToStateID(transComp.m_Facing.m_Direction)
                    });
                    
                }
            }
            else
            {
                SS_DEBUG_LOG("[MOVEMENT SYSTEM] Entity has no transform!")
            }
        }
    }
}