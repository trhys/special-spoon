#pragma once

#include "EntityManager.h"
#include "SFML/Window/Event.hpp"

#include <iostream>

namespace Spoon
{
    class InputSystem
    {
    public:
        InputSystem() {}
        ~InputSystem() {}

        void PushKeyPress(const sf::Event::KeyPressed& keyPress)
        {
            m_KeyPressEvents.push_back(keyPress);
            std::cout << "keypress: " + sf::Keyboard::getDescription(keyPress.scancode).toAnsiString() << std::endl;
        }

        void Update(sf::Time tick, EntityManager& manager)
        {
            // TODO process events
        }
    
    private:
        std::vector<sf::Event::KeyPressed> m_KeyPressEvents;
    };
}