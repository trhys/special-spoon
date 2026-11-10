#include "StateSystem.h"
#include "Core/Application.h"
#include "Core/Registers/ActionRegistry.h"
#include "Utils/StateHelpers.h"

namespace Spoon
{
    void StateSystem::Update(sf::Time tick, EntityManager& manager)
    {
        // preload action queue
        auto& queue = Application::Get().GetActionQueue();

        // get arrays
		auto& statusArray = manager.GetArray<StatusComp>(StatusComp::Name);
		auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);

        for(size_t in = 0; in < statusArray.m_Components.size(); in++)
        {
            StatusComp& stateComp = statusArray.m_Components[in];
            UUID ID = statusArray.m_IndexToId[in];

            // return early on quit request
            if (queue.HasAction(ActionRegistry::Get().GetAction("Quit"), ID))
            {
                Application::Get().HandleQuit();
                return;
            }

            // if status is unresolved - return to idle
            if (!stateComp.resolvedThisFrame)
            {
                if (!transformArray.m_IdToIndex.count(ID)) // default idle
                {
                    stateComp.m_CurrentState = StateType{ BuiltInStates::Idle };
                }
				else { // set idle based on facing direction
                    auto& transformComp = manager.GetComponent<TransformComp>(ID, TransformComp::Name);
                    stateComp.ApplyState(StateType{ DirectionHandler::GetIdleStateID(transformComp.m_Facing.m_Direction) });
                }
            }

            stateComp.resolvedThisFrame = false;
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
}