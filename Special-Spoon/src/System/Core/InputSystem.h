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
        void ResetRuntimeState(EntityManager& manager);
        void Update(sf::Time tick, EntityManager& manager) override;

        // viewport methods
        void SetViewport(sf::RenderTarget* target, sf::FloatRect screenRect, bool hovered);
        void ResolveMouse(const sf::RenderWindow& window);
        std::optional<sf::Vector2f> const GetMousePosition();
    private:
        std::vector<sf::Event::KeyPressed> m_KeyPressEvents;
        std::vector<sf::Event::KeyReleased> m_KeyReleaseEvents;
        std::vector<sf::Event::MouseButtonPressed> m_MouseButtonPressEvents;
        std::vector<sf::Event::MouseButtonReleased> m_MouseButtonReleaseEvents;

        // viewport members
        sf::RenderTarget* m_ActiveViewport = nullptr;  // null expect for editor viewport
        sf::FloatRect m_ViewportRect;
        bool m_ViewportHovered = false;
        std::optional<sf::Vector2f> m_MousePos;
    };
}
