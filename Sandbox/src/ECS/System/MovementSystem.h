#pragma once

#include "Spoon.h"

#include "Core/Application.h"

#include <cmath>
#include <unordered_map>
#include <unordered_set>

class MovementSystem : public Spoon::ISystem
{
public:
    MovementSystem() : Spoon::ISystem::ISystem("Movement") {}
    ~MovementSystem() {}

    static bool IsMovementAction(const Spoon::ActionType& action)
    {
        return action.m_ID == Spoon::BuiltInActions::MoveLeft ||
            action.m_ID == Spoon::BuiltInActions::MoveRight ||
            action.m_ID == Spoon::BuiltInActions::MoveUp ||
            action.m_ID == Spoon::BuiltInActions::MoveDown;
    }

    static void UpdateHeldState(Spoon::MovementComp& moveComp, const Spoon::Action& action)
    {
        bool isPressed = action.m_Event == Spoon::ActionEvent::Pressed;
        switch (action.m_ActionType.m_ID)
        {
        case Spoon::BuiltInActions::MoveLeft:
            moveComp.m_MoveLeftHeld = isPressed;
            break;
        case Spoon::BuiltInActions::MoveRight:
            moveComp.m_MoveRightHeld = isPressed;
            break;
        case Spoon::BuiltInActions::MoveUp:
            moveComp.m_MoveUpHeld = isPressed;
            break;
        case Spoon::BuiltInActions::MoveDown:
            moveComp.m_MoveDownHeld = isPressed;
            break;
        default:
            break;
        }
    }

    static sf::Vector2f BuildIntent(const Spoon::MovementComp& moveComp)
    {
        sf::Vector2f intent(
            (moveComp.m_MoveRightHeld ? 1.0f : 0.0f) - (moveComp.m_MoveLeftHeld ? 1.0f : 0.0f),
            (moveComp.m_MoveDownHeld ? 1.0f : 0.0f) - (moveComp.m_MoveUpHeld ? 1.0f : 0.0f)
        );

        if (intent.x != 0.0f && intent.y != 0.0f)
        {
            constexpr float diagonalScale = 0.70710678f;
            intent *= diagonalScale;
        }

        return intent;
    }

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& queue = Spoon::Application::Get().GetActionQueue();
        auto& movementArray = manager.GetArray<Spoon::MovementComp>(Spoon::MovementComp::Name);
        auto& transformArray = manager.GetArray<Spoon::TransformComp>(Spoon::TransformComp::Name);
        auto& physicsArray = manager.GetArray<Spoon::PhysicsComp>(Spoon::PhysicsComp::Name);
        std::unordered_map<Spoon::UUID, std::vector<const Spoon::Action*>> movementActions;

        for (const auto& action : queue.m_Queue)
        {
            if (IsMovementAction(action.m_ActionType))
            {
                movementActions[action.m_EntityID].push_back(&action);
            }
        }

        for(size_t index = 0; index < movementArray.m_Components.size(); index++)
        {
            Spoon::MovementComp& moveComp = movementArray.m_Components[index];
            Spoon::UUID ID = movementArray.m_IndexToId[index];

            moveComp.m_FrameIntent = {0.0f, 0.0f};
            moveComp.m_ProposedDelta = {0.0f, 0.0f};
            moveComp.m_WasCorrectedByPhysics = false;

            bool receivedMovementAction = false;
            auto actionRange = movementActions.find(ID);
            if (actionRange != movementActions.end())
            {
                for (const Spoon::Action* action : actionRange->second)
                {
                    UpdateHeldState(moveComp, *action);
                    receivedMovementAction = true;
                }
            }

            bool hasHeldMovement = moveComp.m_MoveLeftHeld || moveComp.m_MoveRightHeld ||
                moveComp.m_MoveUpHeld || moveComp.m_MoveDownHeld;

            if (hasHeldMovement)
            {
                moveComp.m_FrameIntent = BuildIntent(moveComp);
                moveComp.m_Velocity = moveComp.m_FrameIntent * moveComp.m_Speed;
                m_ActionDrivenEntities.insert(ID);
                m_LastActionVelocity[ID] = moveComp.m_Velocity;
            }
            else if (receivedMovementAction && m_ActionDrivenEntities.erase(ID) > 0)
            {
                auto lastVelocity = m_LastActionVelocity.find(ID);
                bool velocityMatchesLastAction =
                    lastVelocity != m_LastActionVelocity.end() &&
                    std::abs(moveComp.m_Velocity.x - lastVelocity->second.x) < 0.0001f &&
                    std::abs(moveComp.m_Velocity.y - lastVelocity->second.y) < 0.0001f;

                if (velocityMatchesLastAction)
                {
                    moveComp.m_FrameIntent = { 0.0f, 0.0f };
                    moveComp.m_Velocity = { 0.0f, 0.0f };
                }
                else if (moveComp.m_Velocity.x != 0.0f || moveComp.m_Velocity.y != 0.0f)
                {
                    float magnitude = std::sqrt(moveComp.m_Velocity.x * moveComp.m_Velocity.x + moveComp.m_Velocity.y * moveComp.m_Velocity.y);
                    if (magnitude > 0.0f)
                    {
                        moveComp.m_FrameIntent = moveComp.m_Velocity / magnitude;
                    }
                }

                m_LastActionVelocity.erase(ID);
            }
            else if (moveComp.m_Velocity.x != 0.0f || moveComp.m_Velocity.y != 0.0f)
            {
                float magnitude = std::sqrt(moveComp.m_Velocity.x * moveComp.m_Velocity.x + moveComp.m_Velocity.y * moveComp.m_Velocity.y);
                if (magnitude > 0.0f)
                {
                    moveComp.m_FrameIntent = moveComp.m_Velocity / magnitude;
                }
            }

            if(transformArray.m_IdToIndex.count(ID))
            {
                Spoon::TransformComp& transComp = manager.GetComponent<Spoon::TransformComp>(ID, Spoon::TransformComp::Name);
                moveComp.m_ProposedDelta = moveComp.m_Velocity * tick.asSeconds();
                transComp.Move(moveComp.m_ProposedDelta);

                if (physicsArray.m_IdToIndex.count(ID))
                {
                    Spoon::PhysicsComp& physicsComp = manager.GetComponent<Spoon::PhysicsComp>(ID, Spoon::PhysicsComp::Name);
                    physicsComp.m_CollisionBox.position += moveComp.m_ProposedDelta;
                }

                // Determine direction of travel
                sf::Vector2f facingVector = moveComp.m_FrameIntent;
                if (facingVector.x == 0.0f && facingVector.y == 0.0f)
                {
                    facingVector = moveComp.m_Velocity;
                }

                if(facingVector.x != 0.0f || facingVector.y != 0.0f)
                {
                    moveComp.m_Facing.m_Angle = std::atan2(facingVector.y, facingVector.x) * 180.f / 3.14159f;
                    moveComp.m_Facing.UpdateDirection();
                }
            }
            else
            {
                SS_DEBUG_LOG("[MOVEMENT SYSTEM] Entity has no transform!")
            }
            
        }
    }

private:
    std::unordered_set<Spoon::UUID> m_ActionDrivenEntities;
    std::unordered_map<Spoon::UUID, sf::Vector2f> m_LastActionVelocity;
};
