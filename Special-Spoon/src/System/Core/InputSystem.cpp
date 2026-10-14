#include "InputSystem.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"

namespace Spoon
{
    void InputSystem::Update(sf::Time tick, EntityManager& manager)
    {
        (void)tick; // unused

        // set mouse data
        ResolveMouse(Application::Get().GetWindow());

        // prefetch action queue and arrays
        auto& queue = Application::Get().GetActionQueue();
        auto& inputArray = manager.GetArray<InputComp>(InputComp::Name);

        for(size_t in = 0; in < inputArray.m_Components.size(); in++)
        {
            InputComp& inputComp = inputArray.m_Components[in];
            UUID ID = inputArray.m_IndexToId[in];

			// process key events
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

			// process held keys and queue actions
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

			// process held mouse buttons and queue actions
            for (const auto& [button, isHeld] : inputComp.m_MouseButtonStates)
            {
                if (!isHeld)
                {
                    continue;
                }
                auto binding = inputComp.m_MouseButtonBindings.find(button);
                if (binding != inputComp.m_MouseButtonBindings.end())
                {
                    queue.CreateAndPush(ID, binding->second, 0);
                }
			}
        }

        ClearEvents();
    }

	// Clear all input events after processing
    void InputSystem::ClearEvents()
    {
        m_KeyPressEvents.clear();
        m_KeyReleaseEvents.clear();
        m_MouseButtonPressEvents.clear();
        m_MouseButtonReleaseEvents.clear();
	}

	// mouse + keyboard input event pushing

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
        // get pointer coordinates
        const sf::Vector2i pixel = sf::Mouse::getPosition(window);

        // editor viewport branch
        if (m_ActiveViewport) 
        {
            // mouse out of viewport
            if (!m_ViewportHovered)
            {
                m_MousePos.reset();
                return;
            }

            // get relative position for editor viewport
            sf::Vector2i pos(
                static_cast<int>(pixel.x - m_ViewportRect.position.x),
                static_cast<int>(pixel.y - m_ViewportRect.position.y)    
            );
            m_MousePos = m_ActiveViewport->mapPixelToCoords(pos);
        }
        else // non editor viewport 
        {
            m_MousePos = window.mapPixelToCoords(pixel);
        }
    }

    std::optional<sf::Vector2f> const InputSystem::GetMousePosition()
    {
        return m_MousePos;
    }       
}