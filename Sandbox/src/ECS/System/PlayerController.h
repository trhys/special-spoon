#pragma once

#include "Spoon.h"
#include "ECS/Component/MovementComp.h"
#include "ECS/Component/PlayerComp.h"

class PlayerController : public Spoon::ISystem
{
public:
    PlayerController() {}
    ~PlayerController() {}

    void Update(sf::Time tick, Spoon::EntityManager& manager) override
    {
        auto& playerArray = manager.GetArray<PlayerComp>();
        auto& movementArray = manager.GetArray<MovementComp>();
        auto& statusArray = manager.GetArray<Spoon::StatusComp>();
        auto& actionBuffer = manager.GetActionsBuffer();

        for(size_t index = 0; index < playerArray.m_Components.size(); index++)
        {
            PlayerComp& player = playerArray.m_Components[index];
            Spoon::UUID ID = playerArray.m_IndexToId[index];

            Spoon::StatusComp& status = manager.GetComponent<Spoon::StatusComp>(ID);
            MovementComp& movement = manager.GetComponent<MovementComp>(ID);
            Spoon::InputComp& input = manager.GetComponent<Spoon::InputComp>(ID);

            if (movement.m_Velocity == sf::Vector2f{ 0.0, 0.0 })
            {
                status.m_CurrentState = "Player-IdleFront";
            }
            movement.m_Velocity = { 0.0, 0.0 };

            auto found = actionBuffer.find(ID);
            if (found != actionBuffer.end())
            {
                std::string triggeredAction = found->second;
                status.m_CurrentState = triggeredAction;
            }

            if(status.m_CurrentState == "MoveUp" && input.keyIsPressed)
            {
                movement.m_Velocity.y -= movement.m_Speed;
            }
            else if(status.m_CurrentState == "MoveDown" && input.keyIsPressed)
            {
                movement.m_Velocity.y += movement.m_Speed;
            }
            else if(status.m_CurrentState == "MoveLeft" && input.keyIsPressed)
            {
                movement.m_Velocity.x -= movement.m_Speed;
            }
            else if (status.m_CurrentState == "MoveRight" && input.keyIsPressed)
            {
                movement.m_Velocity.x += movement.m_Speed;
            }
        }
    }
};