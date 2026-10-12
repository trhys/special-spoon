#pragma once

#include "System/System.h"
#include "ECS/ECS.h"

#include "SFML/Window/Event.hpp"

namespace Spoon
{
    class EntityManager;

    class InputSystem : public ISystem
    {
    public:
        InputSystem() : ISystem::ISystem(Name) {}
        
        static constexpr const char* Name = "Input";

        void PushKeyPress(const sf::Event::KeyPressed& keyPress)
        {
            m_KeyPressEvents.push_back(keyPress);
        }

        void PushKeyRelease(const sf::Event::KeyReleased& keyRelease)
        {
            m_KeyReleaseEvents.push_back(keyRelease);
        }

        void PushMouseButtonPress(const sf::Event::MouseButtonPressed& mousePress)
        {
            m_MouseButtonPressEvents.push_back(mousePress);
        }

        void PushMouseButtonRelease(const sf::Event::MouseButtonReleased& mouseRelease)
        {
            m_MouseButtonReleaseEvents.push_back(mouseRelease);
        }

        void Update(sf::Time tick, EntityManager& manager) override;
    
        void ClearEvents()
        {
            m_KeyPressEvents.clear();
            m_KeyReleaseEvents.clear();
            m_MouseButtonPressEvents.clear();
            m_MouseButtonReleaseEvents.clear();
        }
    private:
        std::vector<sf::Event::KeyPressed> m_KeyPressEvents;
        std::vector<sf::Event::KeyReleased> m_KeyReleaseEvents;
        std::vector<sf::Event::MouseButtonPressed> m_MouseButtonPressEvents;
        std::vector<sf::Event::MouseButtonReleased> m_MouseButtonReleaseEvents;
    };
}