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
            if(sceneChangeFlag)
            {
                r_SceneManager.LoadScene(m_CurrentScene, manager, r_SystemManager);
                sceneChangeFlag = false;
            }
        }

        void RequestSceneChange(const std::string& sceneID)
        {
            m_CurrentScene = sceneID;
            sceneChangeFlag = true;
        }

    private:
        SceneManager& r_SceneManager;
        SystemManager& r_SystemManager;
        std::string m_CurrentScene;
        bool sceneChangeFlag = false;
    };
}