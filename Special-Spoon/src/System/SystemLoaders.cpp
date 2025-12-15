#include "SystemLoaders.h"
#include "AnimationSystem.h"
#include "Physics/PhysicsSystem.h"

namespace Spoon
{
    std::unique_ptr<ISystem> LoadAnimationSystem(const json& systemData)
    {
        return std::make_unique<AnimationSystem>();
    }

    std::unique_ptr<ISystem> LoadPhysicsSystem(const json& systemData)
    {
        float x = systemData["GridSize"]["Width"].get<float>();
        float y = systemData["GridSize"]["Height"].get<float>();
        sf::Vector2f gridSize(x, y);
        return std::make_unique<PhysicsSystem>(gridSize);
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