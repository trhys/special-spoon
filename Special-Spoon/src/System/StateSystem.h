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

		// Scene management methods - state system holds the primary scene change and quit flags for the application
		// while the scene manager handles the actual scene change and cleanup
		// Something like the scene manager is the serialized state of the application, while the state system is the runtime state
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