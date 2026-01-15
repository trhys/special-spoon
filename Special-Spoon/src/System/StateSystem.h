#pragma once

#include "System.h"

namespace Spoon
{
    class StateSystem : public ISystem
    {
    public:
        StateSystem() : ISystem::ISystem("State") {}
        ~StateSystem() {}

        void Update(sf::Time tick, EntityManager& manager) override;

        void RequestSceneChange(const std::string& sceneID);
        bool IsSceneChangeRequested();
        std::string ConsumeChangeRequest();
        bool IsQuitRequested();
        void ConsumeQuitFlag();
        
    private:
        std::string m_CurrentScene;
        bool sceneChangeFlag = false;
        bool quitFlag = false;
    };
}