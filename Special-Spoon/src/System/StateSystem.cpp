#include "StateSystem.h"
#include "Core/Application.h"
#include "Core/Registers/ActionRegistry.h"

namespace Spoon
{
    void StateSystem::Update(sf::Time tick, EntityManager& manager)
    {
        auto& queue = Application::Get().GetActionQueue();

        auto& stateActionArray = manager.GetArray<StateActionComp>(StateActionComp::Name);
        for(size_t in = 0; in < stateActionArray.m_Components.size(); in++)
        {
            StateActionComp& stateComp = stateActionArray.m_Components[in];
            UUID ID = stateActionArray.m_IndexToId[in];
            
            auto found = std::find_if(queue.m_Queue.begin(), queue.m_Queue.end(),
                [ID](const Action& action) { return action.m_EntityID == ID; });
            if(found != queue.m_Queue.end())
            {
                if (found->m_ActionType == ActionRegistry::Get().GetAction("Quit"))
                {
                    quitFlag = true;
                    return;
                }
            }
        }

        if(sceneChangeFlag)
        {
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