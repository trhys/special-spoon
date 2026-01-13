#include "InputSystem.h"
#include "Core/Application.h"

namespace Spoon
{
    void InputSystem::Update(sf::Time tick, EntityManager& manager)
    {
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
                    //manager.PushAction(ID, found->second);
                    queue.CreateAndPush(ID, found->second, 0);
                    inputComp.m_KeyStates[found->first] = true;
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
                }
            }
        }
        m_KeyPressEvents.clear();
        m_KeyReleaseEvents.clear();
    }
}