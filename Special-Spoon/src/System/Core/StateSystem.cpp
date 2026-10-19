#include "StateSystem.h"
#include "Core/Application.h"
#include "Core/Registers/ActionRegistry.h"
#include "System/Physics/MovementSystem.h"

namespace Spoon
{
    void StateSystem::Update(sf::Time tick, EntityManager& manager)
    {
        // preload action queue
        auto& queue = Application::Get().GetActionQueue();

        // get arrays
		auto& statusArray = manager.GetArray<StatusComp>(StatusComp::Name);
		auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);

        for(size_t in = 0; in < statusArray.m_Components.size(); in++)
        {
            StatusComp& stateComp = statusArray.m_Components[in];
            UUID ID = statusArray.m_IndexToId[in];
            
            auto found = std::find_if(queue.m_Queue.begin(), queue.m_Queue.end(),
                [ID, &stateComp](const Action& action) { return action.m_EntityID == ID && stateComp.m_Actions.count(action.m_ActionType); });
            if(found != queue.m_Queue.end() && stateComp.m_Actions.count(found->m_ActionType))
            {
                // return early on quit request
                if (found->m_ActionType == ActionRegistry::Get().GetAction("Quit"))
                {
                    Application::Get().HandleQuit();
                    return;
                }

				// special handling of movement actions - these are not mapped to a single state, but rather a direction-based state
                if (MovementSystemUtil::IsMovementAction(found->m_ActionType))
                {
                    if (movementArray.m_IdToIndex.count(ID))
                    {
                        MovementComp& moveComp = movementArray.m_Components[movementArray.m_IdToIndex[ID]];
                        stateComp.m_CurrentState = StateType{
                            MovementSystemUtil::IsMoving(moveComp) ? MovementSystemUtil::DirectionToStateID(moveComp.m_Facing.m_Direction) : BuiltInStates::Idle
                        };
                    }
                    
                    continue;
				}

				// normal action -> state mapping
				stateComp.m_CurrentState = stateComp.m_Actions[found->m_ActionType];
                continue;
            }

            // no action found - return to idle
            stateComp.m_CurrentState = StateType{ BuiltInStates::Idle };
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