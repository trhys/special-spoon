#pragma once

#include "System.h"
#include "ECS/ECS.h"
#include "Core/EntityManager/EntityManager.h"

#include "SFML/Window/Event.hpp"

namespace Spoon
{
    class InputSystem : public ISystem
    {
    public:
        InputSystem() : ISystem::ISystem("Input") {}
        ~InputSystem() {}

        void PushKeyPress(const sf::Event::KeyPressed& keyPress)
        {
            m_KeyPressEvents.push_back(keyPress);
            //std::cout << "keypress: " + sf::Keyboard::getDescription(keyPress.scancode).toAnsiString() << std::endl;
        }

        void PushKeyRelease(const sf::Event::KeyReleased& keyRelease)
        {
            m_KeyReleaseEvents.push_back(keyRelease);
            //std::cout << "key released: " + sf::Keyboard::getDescription(keyRelease.scancode).toAnsiString() << std::endl;
        }

        void Update(sf::Time tick, EntityManager& manager) override;
    
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