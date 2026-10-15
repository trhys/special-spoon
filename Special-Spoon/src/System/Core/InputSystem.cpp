#include "InputSystem.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"

#include <algorithm>
#include <cmath>

namespace Spoon
{
    void InputSystem::Update(sf::Time tick, EntityManager& manager)
    {
        ResolveMouse(Application::Get().GetWindow());

        // fetch queue and arrays
        auto& queue = Application::Get().GetActionQueue();
        auto& inputArray = manager.GetArray<InputComp>(InputComp::Name);
        
        // build delta time
        const float elapsed = tick.asSeconds();
        const float delta = std::isfinite(elapsed) && elapsed > 0.0f ? elapsed : 0.0f;

        for (std::size_t in = 0; in < inputArray.m_Components.size(); ++in)
        {
            InputComp& inputComp = inputArray.m_Components[in];
            const UUID id = inputArray.m_IndexToId[in];

            // iterate this comp's bindings
            for (InputBinding& binding : inputComp.m_Bindings)
            {
                if (!binding.IsValid())
                {
                    binding.ResetRuntimeState();
                    continue;
                }

                // helper to emit an action for this binding
                const auto emitAction = [&queue, id, &binding]
                {
                    if (binding.action)
                        queue.CreateAndPush(id, binding.action, 0);
                };

                // press events
                bool pressedThisUpdate = false;
                const auto* key = std::get_if<sf::Keyboard::Key>(&binding.source);
                const auto* button = std::get_if<sf::Mouse::Button>(&binding.source);
                const bool keyPressed = key
                    && std::any_of(m_KeyPressEvents.begin(), m_KeyPressEvents.end(),
                        [key](const auto& event) { return event.code == *key; });
                const bool mousePressed = button
                    && std::any_of(m_MouseButtonPressEvents.begin(), m_MouseButtonPressEvents.end(),
                        [button](const auto& event) { return event.button == *button; });
                if ((keyPressed || mousePressed) && !binding.isPressed)
                {
                    binding.isPressed = true;
                    binding.timer = 0.0f;
                    pressedThisUpdate = true;
                    if (binding.trigger == InputTrigger::Pressed || binding.trigger == InputTrigger::Repeat)
                        emitAction();
                }

                // release events
                const bool keyReleased = key
                    && std::any_of(m_KeyReleaseEvents.begin(), m_KeyReleaseEvents.end(),
                        [key](const auto& event) { return event.code == *key; });
                const bool mouseReleased = button
                    && std::any_of(m_MouseButtonReleaseEvents.begin(), m_MouseButtonReleaseEvents.end(),
                        [button](const auto& event) { return event.button == *button; });
                if ((keyReleased || mouseReleased) && binding.isPressed)
                {
                    binding.isPressed = false;
                    binding.timer = 0.0f;
                    if (binding.trigger == InputTrigger::Released)
                        emitAction();
                }

                // only continue processing if the binding repeats while held
                if (!binding.isPressed)
                    continue;

                if (binding.trigger == InputTrigger::Held)
                    emitAction();
                else if (binding.trigger == InputTrigger::Repeat && !pressedThisUpdate)
                {
                    binding.timer += delta;
                    if (binding.timer >= binding.repeatIntervalSeconds)
                    {
                        emitAction();
                        binding.timer = std::fmod(binding.timer, binding.repeatIntervalSeconds);
                    }
                }
            }
        }

        ClearEvents();
    }

    void InputSystem::ClearEvents()
    {
        m_KeyPressEvents.clear();
        m_KeyReleaseEvents.clear();
        m_MouseButtonPressEvents.clear();
        m_MouseButtonReleaseEvents.clear();
    }

    void InputSystem::ResetRuntimeState(EntityManager& manager)
    {
        ClearEvents();
        auto& inputArray = manager.GetArray<InputComp>(InputComp::Name);
        for (InputComp& inputComp : inputArray.m_Components)
        {
            for (InputBinding& binding : inputComp.m_Bindings)
                binding.ResetRuntimeState();
        }
    }

    void InputSystem::PushMouseButtonPress(const sf::Event::MouseButtonPressed& mousePress)
    {
        m_MouseButtonPressEvents.push_back(mousePress);
    }

    void InputSystem::PushMouseButtonRelease(const sf::Event::MouseButtonReleased& mouseRelease)
    {
        m_MouseButtonReleaseEvents.push_back(mouseRelease);
    }

    void InputSystem::PushKeyPress(const sf::Event::KeyPressed& keyPress)
    {
        m_KeyPressEvents.push_back(keyPress);
    }

    void InputSystem::PushKeyRelease(const sf::Event::KeyReleased& keyRelease)
    {
        m_KeyReleaseEvents.push_back(keyRelease);
    }

    void InputSystem::SetViewport(sf::RenderTarget* target, sf::FloatRect screenRect, bool hovered)
    {
        m_ActiveViewport = target;
        m_ViewportRect = screenRect;
        m_ViewportHovered = hovered;
    }

    void InputSystem::ResolveMouse(const sf::RenderWindow& window)
    {
        const sf::Vector2i pixel = sf::Mouse::getPosition(window);

        if (m_ActiveViewport)
        {
            if (!m_ViewportHovered)
            {
                m_MousePos.reset();
                return;
            }

            const sf::Vector2i pos(
                static_cast<int>(pixel.x - m_ViewportRect.position.x),
                static_cast<int>(pixel.y - m_ViewportRect.position.y));
            m_MousePos = m_ActiveViewport->mapPixelToCoords(pos);
        }
        else
        {
            m_MousePos = window.mapPixelToCoords(pixel);
        }
    }

    std::optional<sf::Vector2f> const InputSystem::GetMousePosition()
    {
        return m_MousePos;
    }
}
