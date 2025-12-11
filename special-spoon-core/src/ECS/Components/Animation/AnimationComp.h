#pragma once

#include "Core/ResourceManager.h"
#include "ECS/Components/Component.h"
#include "AnimationData.h"

namespace Spoon
{
    struct AnimationComp : public Component
    {
        AnimationComp(std::string animationID, std::unordered_map<std::string, std::string> animationMap = {}) 
            : m_AnimationMap(animationMap)
        {
            m_AnimationData = ResourceManager::GetAnimationData(animationID);
        }

        AnimationData* m_AnimationData = nullptr;
        std::unordered_map<std::string, std::string> m_AnimationMap;

        int currentFrame = 0;
        float elapsedTime = 0.0f;
        bool isFinished = false;

        void SetAnimationData(const std::string animationID)
        {
            m_AnimationData = ResourceManager::GetAnimationData(animationID);
        }
    };
}