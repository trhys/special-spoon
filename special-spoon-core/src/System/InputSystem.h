#pragma once

#include "System.h"
#include "SFML/Window/Event.hpp"

namespace Spoon
{
    class SystemManager;

    class InputSystem : public ISystem
    {
    public:
        InputSystem() {}
        ~InputSystem() {}

        void PushKeyPress(const sf::Event::KeyPressed& keyPress)
        {
            m_KeyPressEvents.push_back(keyPress);
            std::cout << "keypress: " + sf::Keyboard::getDescription(keyPress.scancode).toAnsiString() << std::endl;
        }

        void Update(sf::Time tick, EntityManager& manager) override
        {
            // TODO process events
        }
    
    private:
        std::vector<sf::Event::KeyPressed> m_KeyPressEvents;
    };
}