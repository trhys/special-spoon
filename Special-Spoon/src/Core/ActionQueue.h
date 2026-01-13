#pragma once

#include "ECS/UUID.h"
#include "Registers/ActionRegistry.h"

namespace Spoon
{
    struct Action
    {
        UUID m_EntityID;
        ActionType m_ActionType;
        int m_Priority;
        int m_Timestamp;
    };

    struct ActionQueue
    {
        std::vector<Action> m_Queue;
        int m_NextTimestamp = 0;

        void CreateAndPush(UUID entityID, ActionType actionType, int priority)
        {
            Action action;
            action.m_EntityID = entityID;
            action.m_ActionType = actionType;
            action.m_Priority = priority;
            action.m_Timestamp = m_NextTimestamp++;
            m_Queue.push_back(action);
        }

        void PushAction(Action& action)
        {
            action.m_Timestamp = m_NextTimestamp++;
            m_Queue.push_back(action);
        }

        void Clear()
        {
            m_Queue.clear();
            m_NextTimestamp = 0;
        }
    };
}