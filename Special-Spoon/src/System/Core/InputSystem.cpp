#include "InputSystem.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"

namespace Spoon
{
    void InputSystem::Update(sf::Time tick, EntityManager& manager)
    {
        (void)tick; // unused

        // prefetch action queue and arrays
        auto& queue = Application::Get().GetActionQueue();
        auto& inputArray = manager.GetArray<InputComp>(InputComp::Name);

        for(size_t in = 0; in < inputArray.m_Components.size(); in++)
        {
            InputComp& inputComp = inputArray.m_Components[in];
            UUID ID = inputArray.m_IndexToId[in];

            for(const auto& keyEvent : m_KeyPressEvents)
            {
                sf::Keyboard::Key key = keyEvent.code;
                auto found = inputComp.m_KeyBindings.find(key);
                if(found != inputComp.m_KeyBindings.end())
                {
                    inputComp.m_KeyStates[found->first] = true;
                }
            }

            for (const auto& keyEvent : m_KeyReleaseEvents)
            {
                sf::Keyboard::Key key = keyEvent.code;
                auto found = inputComp.m_KeyBindings.find(key);
                if (found != inputComp.m_KeyBindings.end())
                {
                    inputComp.m_KeyStates[found->first] = false;
                }
            }

            for (const auto& [key, isHeld] : inputComp.m_KeyStates)
            {
                if (!isHeld)
                {
                    continue;
                }

                auto binding = inputComp.m_KeyBindings.find(key);
                if (binding != inputComp.m_KeyBindings.end())
                {
                    queue.CreateAndPush(ID, binding->second, 0);
                }
            }

            // process mouse button events
            for (const auto& mouseEvent : m_MouseButtonPressEvents)
            {
                auto found = inputComp.m_MouseButtonBindings.find(mouseEvent.button);
                if (found != inputComp.m_MouseButtonBindings.end())
                {
                    inputComp.m_MouseButtonStates[found->first] = true;
                }
            }

            for (const auto& mouseEvent : m_MouseButtonReleaseEvents)
            {
                auto found = inputComp.m_MouseButtonBindings.find(mouseEvent.button);
                if (found != inputComp.m_MouseButtonBindings.end())
                {
                    inputComp.m_MouseButtonStates[found->first] = false;
                }
            }
        }

        ClearEvents();
    }
}