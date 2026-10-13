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
		auto& queue = Application::Get().GetActionQueue().GetActionsByEntity();
        auto& uiArray = manager.GetArray<UIComp>(UIComp::Name);

        for(size_t in = 0; in < uiArray.m_Components.size(); in++)
        {
            UIComp& uiComp = uiArray.m_Components[in];
            UUID ID = uiArray.m_IndexToId[in];
            
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
					// Handle UI action
				}
			}
			
		}
    }

    void UISystem::OnReflect()
    {
        // Implementation for reflection
    }
}