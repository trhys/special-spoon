#pragma once

#include "Core/Core.h"
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace Spoon
{
    struct ActionType
    {
        uint32_t m_ID = 0;
        bool operator==(const ActionType& other) const { return m_ID == other.m_ID; }
        bool operator!=(const ActionType& other) const { return m_ID != other.m_ID; }
        explicit operator bool() const { return m_ID != 0; }
    };

    namespace BuiltInActions
    {
        constexpr uint32_t None = 0;
        constexpr uint32_t Quit = 1;
        constexpr uint32_t Idle = 2;

        constexpr uint32_t BeginCustomRange = 1000;
    }

    class SPOON_API ActionRegistry
    {
    public:
        static ActionRegistry& Get()
        {
            static ActionRegistry instance;
            return instance;
        }

        void RegisterAction(const std::string& name, uint32_t id)
        {
            m_NameToID[name] = id;
            m_IDToName[id] = name;
        }

        void RegisterBuiltIns()
        {
            RegisterAction("None", BuiltInActions::None);
            RegisterAction("Quit", BuiltInActions::Quit);
            RegisterAction("Idle", BuiltInActions::Idle);
        }

        void RegisterCustom(const std::string& name)
        {
            RegisterAction(name, m_NextCustomID++);
        }

        ActionType GetAction(const std::string& name)
        {
            if (m_NameToID.find(name) != m_NameToID.end())
            {
                return ActionType{ m_NameToID[name] };
            }
            return ActionType{ BuiltInActions::None };
        }

        ActionType GetFromID(const uint32_t& id)
        {
          if (m_IDToName.find(id) != m_IDToName.end())
          {
            return ActionType{ id };
          }
          throw std::runtime_error("attempted to fetch non existent action type from id");
        }

        std::string& GetName(const ActionType& action)
        {
            return m_IDToName[action.m_ID];
        }

        std::unordered_map<std::string, uint32_t> m_NameToID;
        std::unordered_map<uint32_t, std::string> m_IDToName;
    private:
        ActionRegistry() { RegisterBuiltIns(); }
        uint32_t m_NextCustomID = BuiltInActions::BeginCustomRange;
    };
}

namespace std {
    template <>
    struct hash<Spoon::ActionType> {
        std::size_t operator()(const Spoon::ActionType& action) const noexcept {
            return std::hash<uint32_t>()(action.m_ID);
        }
    };
}

