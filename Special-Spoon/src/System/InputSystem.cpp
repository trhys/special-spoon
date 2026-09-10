#include "InputSystem.h"
#include "Core/Application.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
    static bool IsMovementAction(const ActionType& action)
    {
        return action.m_ID == BuiltInActions::MoveLeft ||
            action.m_ID == BuiltInActions::MoveRight ||
            action.m_ID == BuiltInActions::MoveUp ||
            action.m_ID == BuiltInActions::MoveDown;
    }

    void InputSystem::Update(sf::Time tick, EntityManager& manager)
    {
        (void)tick;
        //manager.ClearActionsBuffer();
        auto& queue = Application::Get().GetActionQueue();
        auto& inputArray = manager.GetArray<InputComp>(InputComp::Name);

        for(size_t in = 0; in < inputArray.m_Components.size(); in++)
        {
            InputComp& inputComp = inputArray.m_Components[in];
            UUID ID = inputArray.m_IndexToId[in];

            for(const auto& keyEvent : m_KeyPressEvents)
            {
                //std::string keyDesc = sf::Keyboard::getDescription(keyEvent.scancode).toAnsiString();
                sf::Keyboard::Key key = keyEvent.code;
                auto found = inputComp.m_KeyBindings.find(key);
                if(found != inputComp.m_KeyBindings.end())
                {
                    inputComp.m_KeyStates[found->first] = true;
                    queue.CreateAndPush(ID, found->second, 0, ActionEvent::Pressed);
                }
                
            }

            for (const auto& keyEvent : m_KeyReleaseEvents)
            {
                //std::string keyDesc = sf::Keyboard::getDescription(keyEvent.scancode).toAnsiString();
                sf::Keyboard::Key key = keyEvent.code;
                auto found = inputComp.m_KeyBindings.find(key);
                if (found != inputComp.m_KeyBindings.end())
                {
                    inputComp.m_KeyStates[found->first] = false;
                    queue.CreateAndPush(ID, found->second, 0, ActionEvent::Released);
                }
            }

            for (const auto& [key, isHeld] : inputComp.m_KeyStates)
            {
                if (!isHeld)
                {
                    continue;
                }

                auto binding = inputComp.m_KeyBindings.find(key);
                if (binding != inputComp.m_KeyBindings.end() && IsMovementAction(binding->second))
                {
                    queue.CreateAndPush(ID, binding->second, 0, ActionEvent::Pressed);
                }
            }
        }
        m_KeyPressEvents.clear();
        m_KeyReleaseEvents.clear();
    }
}