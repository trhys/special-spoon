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

    void UISystem::BuildUIIntent(const std::vector<Action>& actions)
    {
        m_Cursor.intent = UIIntent{};
        for (const auto& action : actions)
        {
            if (UISystemUtil::IsUIAction(action.m_ActionType))
            {
                if (action.m_ActionType.m_ID == BuiltInActions::UISelect)
                    m_Cursor.intent.select = true;
                if (action.m_ActionType.m_ID == BuiltInActions::UICancel)
                    m_Cursor.intent.cancel = true;
                if (action.m_ActionType.m_ID == BuiltInActions::UINavigate)
                    m_Cursor.intent.navigate = true;
            }
        }
    }

    void UISystem::SetCursor(EntityManager& manager)
    {
        // get mouse from input system
        auto mousePosition = Application::Get().GetInputSystem().GetMousePosition();

        // reset hovered component each frame
        m_Cursor.hoveredComp.reset(); 

        // get components arrays from the entity manager
        auto& uiComps = manager.GetArray<UIComp>(UIComp::Name);
        auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
        auto& textArray = manager.GetArray<TextComp>(TextComp::Name);
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
                spriteBounds = spriteComp.GetBoundingBox();
                //spriteActive = &spriteComp.active;
            }
            if (textArray.m_IdToIndex.find(ID) != textArray.m_IdToIndex.end())
            {
                auto& textComp = textArray.m_Components[textArray.m_IdToIndex[ID]];
                textBounds = textComp.GetBoundingBox();
                //textActive = &textComp.active;
            }

            bool hit = uiComp.HitTest(
                mousePosition.value_or(sf::Vector2f{}), 
                spriteBounds ? &(*spriteBounds) : nullptr, 
                textBounds ? &(*textBounds) : nullptr
            );

            if (hit) {
                m_Cursor.hoveredComp = ID;
                return;
            }
        }
    }


    void UISystem::Update(sf::Time tick, EntityManager& manager)
    {
        (void)tick;

        SetCursor(manager);

        auto& queue = Application::Get().GetActionQueue();
        BuildUIIntent(queue.GetQueue());

        auto& uiArray = manager.GetArray<UIComp>(UIComp::Name);

        auto firstActiveUI = [&]() -> UUID {
            for (size_t i = 0; i < uiArray.m_Components.size(); ++i)
            {
                if (uiArray.m_Components[i].active)
                    return uiArray.m_IndexToId[i];
            }
            return UUID{};
        };

        auto nextActiveUI = [&](UUID current) -> UUID {
            if (uiArray.m_Components.empty())
                return UUID{};

            int currentIndex = -1;
            for (size_t i = 0; i < uiArray.m_Components.size(); ++i)
            {
                if (uiArray.m_IndexToId[i] == current)
                {
                    currentIndex = static_cast<int>(i);
                    break;
                }
            }

            const size_t n = uiArray.m_Components.size();
            if (currentIndex < 0)
                return firstActiveUI();

            for (size_t step = 1; step <= n; ++step)
            {
                size_t idx = (static_cast<size_t>(currentIndex) + step) % n;
                if (uiArray.m_Components[idx].active)
                    return uiArray.m_IndexToId[idx];
            }

            return UUID{};
        };

        auto isActiveUI = [&](UUID id) -> bool {
            if (id == UUID{}) return false;
            auto it = uiArray.m_IdToIndex.find(id);
            if (it == uiArray.m_IdToIndex.end())
                return false;
            return uiArray.m_Components[it->second].active;
        };

        if (!isActiveUI(m_Cursor.focusedComp.value_or(UUID{})))
            m_Cursor.focusedComp.reset();
        if (!isActiveUI(m_Cursor.pressedComp.value_or(UUID{})))
            m_Cursor.pressedComp.reset();

        if (isActiveUI(m_Cursor.hoveredComp.value_or(UUID{})))
            m_Cursor.focusedComp = m_Cursor.hoveredComp;

        if (m_Cursor.intent.navigate)
        {
            if (!isActiveUI(m_Cursor.focusedComp.value_or(UUID{})))
                m_Cursor.focusedComp = firstActiveUI();
            else
                m_Cursor.focusedComp = nextActiveUI(m_Cursor.focusedComp.value_or(UUID{}));
        }

        const bool selectHeld = m_Cursor.intent.select;
        const bool selectDown =  selectHeld && !m_Cursor.selectHeldLastFrame;
        const bool selectUp   = !selectHeld &&  m_Cursor.selectHeldLastFrame;

        if (selectDown)
            m_Cursor.pressedComp = m_Cursor.focusedComp;

        if (selectUp)
        {
            if (isActiveUI(m_Cursor.pressedComp.value_or(UUID{})) &&
                m_Cursor.pressedComp == m_Cursor.focusedComp)
            {
                auto it = uiArray.m_IdToIndex.find(m_Cursor.focusedComp.value());
                if (it != uiArray.m_IdToIndex.end())
                {
                    UIComp& focusedUI = uiArray.m_Components[it->second];
                    if (focusedUI.emittedAction)
                    {
                        queue.CreateAndPush(m_Cursor.focusedComp.value(), focusedUI.emittedAction, 0);
                    }
                }
            }
            m_Cursor.pressedComp.reset();
        }

        if (m_Cursor.intent.cancel)
        {
            m_Cursor.pressedComp.reset();
            m_Cursor.focusedComp.reset();
        }

        m_Cursor.selectHeldLastFrame = selectHeld;

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