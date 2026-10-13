#pragma once

#include "System/System.h"

#include "SFML/Window/Event.hpp"

namespace Spoon
{
    class EntityManager;

    class InputSystem : public ISystem
    {
    public:
        InputSystem() : ISystem::ISystem(Name) {}
        
        static constexpr const char* Name = "Input";

        void PushKeyPress(const sf::Event::KeyPressed& keyPress);
        void PushKeyRelease(const sf::Event::KeyReleased& keyRelease);
        void PushMouseButtonPress(const sf::Event::MouseButtonPressed& mousePress);
        void PushMouseButtonRelease(const sf::Event::MouseButtonReleased& mouseRelease);
        void ClearEvents();
        
        void Update(sf::Time tick, EntityManager& manager) override;
    private:
        std::vector<sf::Event::KeyPressed> m_KeyPressEvents;
        std::vector<sf::Event::KeyReleased> m_KeyReleaseEvents;
        std::vector<sf::Event::MouseButtonPressed> m_MouseButtonPressEvents;
        std::vector<sf::Event::MouseButtonReleased> m_MouseButtonReleaseEvents;
    };
}