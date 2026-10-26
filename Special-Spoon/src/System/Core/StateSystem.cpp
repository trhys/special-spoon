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

            // return early on quit request
            if (queue.HasAction(ActionRegistry::Get().GetAction("Quit"), ID))
            {
                Application::Get().HandleQuit();
                return;
            }

            // if status is unresolved - return to idle
            if (!stateComp.resolvedThisFrame)
                stateComp.m_CurrentState = StateType{ BuiltInStates::Idle };
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