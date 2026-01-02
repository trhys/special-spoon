#include "SystemLoaders.h"
#include "System/AnimationSystem.h"
#include "System/Physics/PhysicsSystem.h"

namespace Spoon
{
    std::unique_ptr<ISystem> LoadAnimationSystem(const json* systemData)
    {
        return std::make_unique<AnimationSystem>();
    }

    std::unique_ptr<ISystem> LoadPhysicsSystem(const json* systemData)
    {
        if(systemData)
        {
            // UPDATE: I'm thinking we'll move this setup to an OnInit() method
            // in the physics system itself. Should get a config from somewhere
            // else so it's changeable in the editor
            
            // float x = systemData["GridSize"]["Width"].get<float>();
            // float y = systemData["GridSize"]["Height"].get<float>();
            // sf::Vector2f gridSize(x, y);
        }
        
        return std::make_unique<PhysicsSystem>();
    }

    void RegisterDefaultSystems()
    {
        SS_DEBUG_LOG("[SYSTEM] Registering default systems...")
        SystemLoaders::RegisterSysLoader("Animation", &LoadAnimationSystem);
        SystemLoaders::RegisterSysLoader("Physics", &LoadPhysicsSystem);
    }

    namespace
    {
        class RegisterDefaultSystemsHelper
        {
        public:
            RegisterDefaultSystemsHelper()
            {
                RegisterDefaultSystems();
            }
        };

        static RegisterDefaultSystemsHelper s_RegisterDefaultSystemsHelper;
    }
}