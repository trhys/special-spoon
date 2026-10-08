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
            for (const auto& action : queue)
            {
                // todo
            }
        }
    }

    void AudioSystem::OnReflect()
    {
        // Implementation of the reflection logic for the audio system
    }
}