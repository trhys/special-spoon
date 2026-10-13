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
        auto& inputArray = manager.GetArray<InputComp>(InputComp::Name);
        auto& spriteArray = manager.GetArray<SpriteComp>(SpriteComp::Name);
        auto& textArray = manager.GetArray<TextComp>(TextComp::Name);

        for(size_t in = 0; in < uiArray.m_Components.size(); in++)
        {
            UIComp& uiComp = uiArray.m_Components[in];
            UUID ID = uiArray.m_IndexToId[in];

            // get input component for the entity
            // and update its state from mouse position
            if (inputArray.m_IdToIndex.find(ID) == inputArray.m_IdToIndex.end())
                continue;
            auto& inputComp = inputArray.m_Components[inputArray.m_IdToIndex[ID]];
            sf::Vector2f mousePosition = inputComp.GetMousePosition();

            // get sprite/text for the UI element - 
            // we'll hold its bounds and active state for later
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
                mousePosition, 
                spriteBounds ? &(*spriteBounds) : nullptr, 
                textBounds ? &(*textBounds) : nullptr
            );
            
            // reset active state before checking
            uiComp.ClearActiveState(spriteActive, textActive);

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
					
				}
			}
			
		}
    }

    void UISystem::OnReflect()
    {
        // Implementation for reflection
    }
}