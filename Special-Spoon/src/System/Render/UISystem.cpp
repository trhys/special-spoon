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


    void UISystem::Update(sf::Time tick, EntityManager& manager)
    {
		// prefetch queue and arrays
		auto queue = Application::Get().GetActionQueue().GetActionsByEntity();
        auto& uiArray = manager.GetArray<UIComp>(UIComp::Name);
        auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
        auto& textArray = manager.GetArray<TextComp>(TextComp::Name);

        for(size_t in = 0; in < uiArray.m_Components.size(); in++)
        {
            UIComp& uiComp = uiArray.m_Components[in];
            UUID ID = uiArray.m_IndexToId[in];
            
            // get sprite/text for the UI element - 
            // we'll hold its bounds and active state for hit testing
            std::optional<sf::FloatRect> spriteBounds;
            bool* spriteActive = nullptr;
            std::optional<sf::FloatRect> textBounds;
            bool* textActive = nullptr;

            if (spriteArray.m_IdToIndex.find(ID) != spriteArray.m_IdToIndex.end())
            {
                auto& spriteComp = spriteArray.m_Components[spriteArray.m_IdToIndex[ID]];
                spriteBounds = spriteComp.GetBoundingBox();
                spriteActive = &spriteComp.active;
            }
            if (textArray.m_IdToIndex.find(ID) != textArray.m_IdToIndex.end())
            {
                auto& textComp = textArray.m_Components[textArray.m_IdToIndex[ID]];
                textBounds = textComp.GetBoundingBox();
                textActive = &textComp.active;
            }

            // update UI component's hover state
            uiComp.HitTest(
                sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y), 
                spriteBounds ? &(*spriteBounds) : nullptr, 
                textBounds ? &(*textBounds) : nullptr
            );
            // update the UICursor with the hover state
            m_Cursor.UpdateHover(&uiComp);

			auto it = queue.find(ID);
            if (it == queue.end())
                continue;
            auto& actions = it->second;  // a vector of actions for this entity
            if (actions.empty())
				continue;

            for (const auto& action : actions)
            {
				if (UISystemUtil::IsUIAction(action.m_ActionType))
				{
					if (action.m_ActionType.m_ID == BuiltInActions::UISelect && (uiComp.isHovered || uiComp.isFocused))
						m_Cursor.UpdatePressed(&uiComp);
                    if (action.m_ActionType.m_ID == BuiltInActions::UICancel && (uiComp.isHovered || uiComp.isFocused))
                        m_Cursor.UpdatePressed(nullptr);
                    if (action.m_ActionType.m_ID == BuiltInActions::UINavigate)
                        m_Cursor.UpdateFocus(&uiComp);
				}
			}
			
		}
    }

    void UISystem::OnReflect()
    {
        // Implementation for reflection
    }
}