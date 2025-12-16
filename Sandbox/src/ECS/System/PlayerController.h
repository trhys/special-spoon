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

            movement.m_Velocity = { 0.0, 0.0 };

            auto found = actionBuffer.find(ID);
            if (found != actionBuffer.end())
            {
                std::string triggeredAction = found->second;
                status.m_CurrentState = triggeredAction;
            }

            for(const auto& keyState : input.m_KeyStates)
            {
                if(keyState.second)
                {
                    std::string keypressed = keyState.first;
                    std::string action = input.m_KeyBindings[keypressed];

                    if(action == "MoveUp")
                    {
                        movement.m_Velocity.y -= movement.m_Speed;
                    }
                    else if(action == "MoveDown")
                    {
                        movement.m_Velocity.y += movement.m_Speed;
                    }
                    else if(action == "MoveLeft")
                    {
                        movement.m_Velocity.x -= movement.m_Speed;
                    }
                    else if (action == "MoveRight")
                    {
                        movement.m_Velocity.x += movement.m_Speed;
                    }
                }
            }
            if (movement.m_Velocity == sf::Vector2f{ 0.0, 0.0 })
            {
                status.m_CurrentState = "Player-IdleFront";
            }
        }
    }
};