#pragma once

#include "System.h"

namespace Spoon
{
    class SceneManager;
    class SystemManager;

    class StateSystem : public ISystem
    {
    public:
        StateSystem(SceneManager& manager, SystemManager& system) : r_SceneManager(manager), r_SystemManager(system) {}
        ~StateSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override
        {
            auto& stateActionArray = manager.GetArray<StateActionComp>();
            for(size_t in = 0; in < stateActionArray.m_Components.size(); in++)
            {
                StateActionComp& stateComp = stateActionArray.m_Components[in];
                UUID ID = stateActionArray.m_IndexToId[in];

                auto& actionsBuffer = manager.GetActionsBuffer();
                auto found = actionsBuffer.find(ID);
                if(found != actionsBuffer.end())
                {
                    std::string triggeredAction = found->second;
                    if(stateComp.m_Actions.count(triggeredAction))
                    {
                        std::string targetState = stateComp.m_Actions[triggeredAction];
                        if(targetState == "Quit")
                        {
                            quitFlag = true;
                            return;
                        }
                        RequestSceneChange(targetState);
                        break;
                    }
                }
            }

            if(sceneChangeFlag)
            {
                return;
            }
        }

        void RequestSceneChange(const std::string& sceneID)
        {
            m_CurrentScene = sceneID;
            sceneChangeFlag = true;
        }

        bool IsQuitRequested() { return quitFlag; }
        bool IsSceneChangeRequested() { return sceneChangeFlag; }
        std::string ConsumeChangeRequest() 
        { 
            sceneChangeFlag = false;
            return m_CurrentScene; 
        }

    private:
        SceneManager& r_SceneManager;
        SystemManager& r_SystemManager;
        std::string m_CurrentScene;
        bool sceneChangeFlag = false;
        bool quitFlag = false;
    };
}