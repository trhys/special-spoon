#pragma once

#include "ECS/Components/Component.h"
#include "AnimationData.h"

namespace Spoon
{
    struct AnimationComp : public ComponentBase<AnimationComp>
    {
        AnimationComp() : ComponentBase::ComponentBase(Name) {}

        static constexpr const char* Name = "Animation";

        AnimationData* m_AnimationData = nullptr;
        std::string iAnimationID;
        std::unordered_map<std::string, std::string> m_AnimationMap; // [key, value] = [state, animationID]

        int currentFrame = 0;
        float elapsedTime = 0.0f;
        bool isFinished = false;

        void SetAnimationData(const std::string animationID);
        void OnReflect() override;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationComp, iAnimationID, m_AnimationMap)
}