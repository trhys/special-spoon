#pragma once

#include "ECS/UUID.h"
#include "Registers/ActionRegistry.h"
#include <unordered_map>

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

        const std::vector<Action>& GetQueue() const { return m_Queue; }
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

        std::unordered_map<UUID, std::vector<Action>> GetActionsByEntity() const
        {
            std::unordered_map<UUID, std::vector<Action>> actionsByEntity;
            for (const auto& action : m_Queue)
            {
                actionsByEntity[action.m_EntityID].push_back(action);
            }
            return actionsByEntity;
		}

        // check if entity has a specific action on the queue
        bool HasAction(ActionType actionType, UUID id) const
        {
            for (const auto& action : m_Queue)
            {
                if (action.m_EntityID == id && action.m_ActionType == actionType)
                    return true;
            }
            return false;
        }
    };
}