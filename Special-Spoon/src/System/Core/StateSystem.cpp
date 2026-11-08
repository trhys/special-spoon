#include "StateSystem.h"
#include "Core/Application.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
	uint32_t DirectionToIdleStateID(const char* direction); // forward declaration

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
                    SS_DEBUG_LOG("[STATE SYSTEM] Entity has no transform component!");
                    stateComp.m_CurrentState = StateType{ BuiltInStates::Idle };
                }
				else { // set idle based on facing direction
                    auto& transformComp = manager.GetComponent<TransformComp>(ID, TransformComp::Name);
                    stateComp.ApplyState(StateType{ DirectionToIdleStateID(transformComp.m_Facing.m_Direction) });
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

    // idle direction helper
    uint32_t DirectionToIdleStateID(const char* direction)
    {
        if (strcmp(direction, "Up") == 0)
            return BuiltInStates::IdleUp;
        else if (strcmp(direction, "Down") == 0)
            return BuiltInStates::IdleDown;
        else if (strcmp(direction, "Left") == 0)
            return BuiltInStates::IdleLeft;
        else if (strcmp(direction, "Right") == 0)
            return BuiltInStates::IdleRight;
        else if (strcmp(direction, "UpRight") == 0)
            return BuiltInStates::IdleUR;
        else if (strcmp(direction, "UpLeft") == 0)
            return BuiltInStates::IdleUL;
        else if (strcmp(direction, "DownRight") == 0)
            return BuiltInStates::IdleDR;
        else if (strcmp(direction, "DownLeft") == 0)
            return BuiltInStates::IdleDL;
        else
            return BuiltInStates::Idle;
	}
}