#include "StateSystem.h"
#include "Core/Application.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
    void StateSystem::Update(sf::Time tick, EntityManager& manager)
    {
        // preload action queue
        auto& queue = Application::Get().GetActionQueue();

        // get arrays
		auto& statusArray = manager.GetArray<StatusComp>(StatusComp::Name);

        for(size_t in = 0; in < statusArray.m_Components.size(); in++)
        {
            StatusComp& stateComp = statusArray.m_Components[in];
            UUID ID = statusArray.m_IndexToId[in];
            
            auto found = std::find_if(queue.m_Queue.begin(), queue.m_Queue.end(),
                [ID](const Action& action) { return action.m_EntityID == ID; });
            if(found != queue.m_Queue.end() && stateComp.m_Actions.count(found->m_ActionType))
            {
				stateComp.m_CurrentState = stateComp.m_Actions[found->m_ActionType];
                if (found->m_ActionType == ActionRegistry::Get().GetAction("Quit"))
                {
                    quitFlag = true;
                    return;
                }
            }
        }

        if(sceneChangeFlag)
        {
			// todo: handle scene change logic 
            return;
        }
    }

    void StateSystem::RequestSceneChange(const std::string& sceneID)
    {
        m_CurrentScene = sceneID;
        sceneChangeFlag = true;
    }

    std::string StateSystem::ConsumeChangeRequest() 
    { 
        sceneChangeFlag = false;
        return m_CurrentScene; 
    }

    bool StateSystem::IsSceneChangeRequested() { return sceneChangeFlag; }
    bool StateSystem::IsQuitRequested() { return quitFlag; }
    void StateSystem::ConsumeQuitFlag() { quitFlag = false; }
}