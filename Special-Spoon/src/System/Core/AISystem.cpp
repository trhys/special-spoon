#include "AISystem.h"
#include "Core/Application.h"
#include "Core/EntityManager/EntityManager.h"

namespace Spoon
{
    void AISystem::Update(sf::Time tick, EntityManager& manager)
    {
        // prefetch action queue and arrays
        auto& actionQueue = Application::Get().GetActionQueue();
        auto& aiArray = manager.GetArray<AIComponent>(AIComponent::Name);

        for (auto& [id, index] : aiArray.m_IdToIndex)
        {
            AIComponent& aiComp = aiArray.m_Components[index];
            m_Lua.script_file(aiComp.m_ActiveScript.string());
        }
    }
}