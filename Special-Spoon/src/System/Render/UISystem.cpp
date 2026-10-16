#include "UISystem.h"
#include "Core/Application.h"

namespace Spoon
{
	namespace UISystemUtil
    {
        bool IsUIAction(const ActionType& action)
        {
            // Define what constitutes a UI action
            return action.m_ID == BuiltInActions::UISelect ||
                   action.m_ID == BuiltInActions::UICancel ||
                   action.m_ID == BuiltInActions::UINavigate;
        }
    }

    void UISystem::SetCursor(EntityManager& manager)
    {
        // get mouse from input system
        auto mousePosition = Application::Get().GetInputSystem().GetMousePosition();
        if (!mousePosition.has_value())
        {
            m_Cursor.hoveredComp.reset();
            return;
        }

        // reset hovered component each frame
        m_Cursor.hoveredComp.reset(); 

        // get components arrays from the entity manager
        auto& uiComps = manager.GetArray<UIComp>(UIComp::Name);
        auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
        auto& textArray = manager.GetArray<TextComp>(TextComp::Name);

        std::vector<UUID> candidates;
        for(size_t in = 0; in < uiComps.m_Components.size(); in++)
        {
            UIComp& uiComp = uiComps.m_Components[in];
            if (!uiComp.active)
                continue;

            UUID ID = uiComps.m_IndexToId[in];

            std::optional<sf::FloatRect> spriteBounds;
            //bool* spriteActive = nullptr;
            std::optional<sf::FloatRect> textBounds;
            //bool* textActive = nullptr;

            if (spriteArray.m_IdToIndex.find(ID) != spriteArray.m_IdToIndex.end())
            {
                auto& spriteComp = spriteArray.m_Components[spriteArray.m_IdToIndex[ID]];
                if (!spriteComp.active)
                    continue;
                spriteBounds = spriteComp.GetBoundingBox();
                //spriteActive = &spriteComp.active;
            }
            if (textArray.m_IdToIndex.find(ID) != textArray.m_IdToIndex.end())
            {
                auto& textComp = textArray.m_Components[textArray.m_IdToIndex[ID]];
                if (!textComp.active)
                    continue;
                textBounds = textComp.GetBoundingBox();
                //textActive = &textComp.active;
            }

            if (uiComp.HitTest(
                mousePosition.value_or(sf::Vector2f{}), 
                spriteBounds ? &(*spriteBounds) : nullptr, 
                textBounds ? &(*textBounds) : nullptr
            ))
            {
                candidates.push_back(ID);
            }
        }
        // sort candidates and pick the best hit
        if (!candidates.empty())
            m_Cursor.hoveredComp = PickBestHit(SortCandidates(candidates, manager));
    }

    std::vector<UUID> UISystem::SortCandidates(std::vector<UUID> candidates, EntityManager& manager)
    {
        // get ui array and layers
        auto& uiArray = manager.GetArray<UIComp>(UIComp::Name);
        auto& layerArray = manager.GetArray<RenderLayer>(RenderLayer::Name);
        if (candidates.empty())
            return candidates;

        // replicate the render order to determine what gets drawn on top
        std::sort(candidates.begin(), candidates.end(), [&](const UUID& a, const UUID& b) {
            // get comps for both IDs
            auto& uiA = uiArray.m_Components[uiArray.m_IdToIndex[a]];
            auto& uiB = uiArray.m_Components[uiArray.m_IdToIndex[b]];
            auto& layerA = layerArray.m_Components[layerArray.m_IdToIndex[a]];
            auto& layerB = layerArray.m_Components[layerArray.m_IdToIndex[b]];

            // sort based on UISpace (Screen space on top of World space)
            // render layer tiebreaker, and finally by id
            return std::tie(uiA.space, layerA.m_Layer, a) < std::tie(uiB.space, layerB.m_Layer, b);
        });
        return candidates;
    }

    UUID UISystem::PickBestHit(const std::vector<UUID>& candidates)
    {
        // pick the best hit based on the sorted order
        return candidates.empty() ? UUID{} : candidates.back();
    }

    void UISystem::Update(sf::Time tick, EntityManager& manager)
    {
        (void)tick;

        SetCursor(manager);

        auto& queue = Application::Get().GetActionQueue();
        auto& uiArray = manager.GetArray<UIComp>(UIComp::Name);

        // move cursor to next active UI component
        auto nextActiveUI = [&](UUID current) -> std::optional<UUID> {
            if (uiArray.m_Components.empty()) // no components available
                return std::nullopt;
            else if (uiArray.m_Components.size() == 1) // only one component, return it
            {
                if (uiArray.m_Components[0].active)
                    return uiArray.m_IndexToId[0];
                else
                    return std::nullopt;
            }

            auto index = uiArray.m_IdToIndex.find(current);
            if (index == uiArray.m_IdToIndex.end())
                if (uiArray.m_Components[0].active)
                    return uiArray.m_IndexToId[0]; // return to start if current not found
                else
                    return std::nullopt;

            // step through the array to find the next active component
            // wrapping around the array if necessary
            auto i = (index->second + 1) % uiArray.m_Components.size();
            for (size_t count = 0; count < uiArray.m_Components.size(); ++count)
            {
                if (uiArray.m_Components[i].active)
                    return uiArray.m_IndexToId[i];
                i = (i + 1) % uiArray.m_Components.size();
            }

            return std::nullopt; // no active component found
        };

        // check if the given UI component ID corresponds to an active component
        auto isActiveUI = [&](std::optional<UUID> id) -> bool {
            if (id == std::nullopt) return false;
            auto it = uiArray.m_IdToIndex.find(id.value());
            if (it == uiArray.m_IdToIndex.end())
                return false;
            return uiArray.m_Components[it->second].active;
        };

        // reset cursor if the associated UI component is no longer active
        if (!isActiveUI(m_Cursor.focusedComp))
        {
            m_Cursor.focusedComp.reset();
            if (m_Cursor.mode == CursorMode::Keyboard)
                m_Cursor.mode = CursorMode::Pointer;
        }

		// hovered component becomes focused if it is active and cursor is in pointer mode
        if (isActiveUI(m_Cursor.hoveredComp) && m_Cursor.mode == CursorMode::Pointer)
            m_Cursor.focusedComp = m_Cursor.hoveredComp;

        // build frame intent for ui commands
        bool uiSelect = false, uiCancel = false, uiNavigate = false;
        for (auto& action : queue.GetQueue())
        {
            if (action.m_ActionType.m_ID == BuiltInActions::UISelect)
                uiSelect = true;
            else if (action.m_ActionType.m_ID == BuiltInActions::UICancel)
                uiCancel = true;
            else if (action.m_ActionType.m_ID == BuiltInActions::UINavigate)
                uiNavigate = true;
        }

        if (uiCancel)
        {
            // reset cursor
            if (m_Cursor.focusedComp)
				m_Cursor.Reset();
        } 
        else
        {
            auto focusedCompId = m_Cursor.focusedComp;
            if (uiSelect && focusedCompId)
            {
				// if a component is focused, emit its action
                auto& comp = uiArray.m_Components[uiArray.m_IdToIndex[focusedCompId.value()]];
                queue.CreateAndPush(focusedCompId.value(), comp.emittedAction, 0);
            } else if (uiNavigate)
            {
                // set cursor mode
				m_Cursor.mode = CursorMode::Keyboard;
                if (!focusedCompId)
                {
                    if (!uiArray.m_Components.empty())
                        m_Cursor.focusedComp = uiArray.m_IndexToId[0];
                } else {
                    m_Cursor.focusedComp = nextActiveUI(focusedCompId.value());
                }
            } else if (m_Cursor.mode != CursorMode::Keyboard)
            {
				// if not navigating with keyboard, reset focused component when no longer hovered
                if (!m_Cursor.hoveredComp || !isActiveUI(m_Cursor.hoveredComp))
					m_Cursor.focusedComp.reset();
			}
        }

        // Mirror cursor state onto components for debug/render/editor
        for (size_t i = 0; i < uiArray.m_Components.size(); ++i)
        {
            UIComp& uiComp = uiArray.m_Components[i];
            UUID id = uiArray.m_IndexToId[i];

            if (!uiComp.active)
            {
                uiComp.isHovered = false;
                uiComp.isFocused = false;
                continue;
            }

            uiComp.isHovered = (m_Cursor.hoveredComp && id == m_Cursor.hoveredComp.value());
            uiComp.isFocused = (m_Cursor.focusedComp && id == m_Cursor.focusedComp.value());
        }
    }

    void UISystem::OnReflect()
    {
        // Implementation for reflection
    }
}