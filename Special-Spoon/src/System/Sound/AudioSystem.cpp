#include "AudioSystem.h"
#include "Core/Application.h"

namespace Spoon
{
    void AudioSystem::Update(sf::Time tick, EntityManager& manager)
    {
        // prefetch action queue for the frame
        auto& queue = Application::Get().GetActionQueue();

        // get all audio comps
        auto& audioArray = manager.GetArray<AudioComp>(AudioComp::Name);
        for (size_t index = 0; index < audioArray.m_Components.size(); ++index)
        {
            auto& audioComp = audioArray.m_Components[index];
            UUID id = audioArray.m_IndexToId[index];
            
            // check action queue for audio cue
            
            for (const auto& action : queue.m_Queue)
            {
                uint32_t actionId = action.m_ActionType.m_ID;
                if (action.m_EntityID == id)
                {
                  if (audioComp.IsActionMapped(actionId))
                  {
                    audioComp.LoadSound(audioComp.actionBuffers[actionId]);
                    audioComp.PlaySound();
                  }
                }
            }
        }
    }

    void AudioSystem::OnReflect()
    {
        // Implementation of the reflection logic for the audio system
    }
}
