#pragma once

#include "System.h"
#include "ECS/ECS.h"
#include "Core/EntityManager/EntityManager.h"

#include "SFML/Window/Event.hpp"

namespace Spoon
{
    class SystemManager;

    class InputSystem : public ISystem
    {
    public:
        InputSystem() : ISystem::ISystem("Input") {}
        ~InputSystem() {}

        void PushKeyPress(const sf::Event::KeyPressed& keyPress)
        {
            m_KeyPressEvents.push_back(keyPress);
            std::cout << "keypress: " + sf::Keyboard::getDescription(keyPress.scancode).toAnsiString() << std::endl;
        }

        void PushKeyRelease(const sf::Event::KeyReleased& keyRelease)
        {
            m_KeyReleaseEvents.push_back(keyRelease);
            std::cout << "key released: " + sf::Keyboard::getDescription(keyRelease.scancode).toAnsiString() << std::endl;
        }

        void Update(sf::Time tick, EntityManager& manager) override
        {
            manager.ClearActionsBuffer();
            auto& inputArray = manager.GetArray<InputComp>();

            for(size_t in = 0; in < inputArray.m_Components.size(); in++)
            {
                InputComp& inputComp = inputArray.m_Components[in];
                UUID ID = inputArray.m_IndexToId[in];

                for(const auto& keyEvent : m_KeyPressEvents)
                {
                    std::string keyDesc = sf::Keyboard::getDescription(keyEvent.scancode).toAnsiString();
                    auto found = inputComp.m_KeyBindings.find(keyDesc);
                    if(found != inputComp.m_KeyBindings.end())
                    {
                        manager.PushAction(ID, found->second);
                        inputComp.m_KeyStates[found->first] = true;
                    }
                    
                }

                for (const auto& keyEvent : m_KeyReleaseEvents)
                {
                    std::string keyDesc = sf::Keyboard::getDescription(keyEvent.scancode).toAnsiString();
                    auto found = inputComp.m_KeyBindings.find(keyDesc);
                    if (found != inputComp.m_KeyBindings.end())
                    {
                        inputComp.m_KeyStates[found->first] = false;
                    }
                }
            }
            m_KeyPressEvents.clear();
            m_KeyReleaseEvents.clear();
        }
    
        void ClearEvents()
        {
            m_KeyPressEvents.clear();
            m_KeyReleaseEvents.clear();
        }
    private:
        std::vector<sf::Event::KeyPressed> m_KeyPressEvents;
        std::vector<sf::Event::KeyReleased> m_KeyReleaseEvents;
    };
}