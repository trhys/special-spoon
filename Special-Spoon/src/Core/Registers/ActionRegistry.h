#pragma once

#include "Core/Core.h"
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace Spoon
{
	// ActionType is a wrapper around a uint32_t ID that represents an action in the system.
	// We use this as a type-safe way to represent actions, and to allow for easy comparison and hashing.
	// ActionQueue stores an action type and an associated entity ID, allowing systems to queue actions for processing.
    struct ActionType
    {
        uint32_t m_ID = 0;
        bool operator==(const ActionType& other) const { return m_ID == other.m_ID; }
        bool operator!=(const ActionType& other) const { return m_ID != other.m_ID; }
        explicit operator bool() const { return m_ID != 0; }
    };

	// BuiltInActions namespace contains a set of predefined action IDs that are used throughout the system.
	// Because these are built-in, they have fixed IDs and should not be changed. Custom actions can be registered starting from BeginCustomRange.
    namespace BuiltInActions
    {
        constexpr uint32_t None = 0;
        constexpr uint32_t Quit = 1;
        constexpr uint32_t Pause = 2;
		constexpr uint32_t UISelect = 4;
		constexpr uint32_t UICancel = 5;
		constexpr uint32_t UINavigate = 6;
        constexpr uint32_t Idle = 7;
        constexpr uint32_t MoveLeft = 8;
        constexpr uint32_t MoveRight = 9;
        constexpr uint32_t MoveUp = 10;
        constexpr uint32_t MoveDown = 11;
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

		// Initialize the registry from the built-in namespace - called from the constructor to ensure built-in actions are always registered.
        void RegisterBuiltIns()
        {
            RegisterAction("None", BuiltInActions::None);
            RegisterAction("Quit", BuiltInActions::Quit);
            RegisterAction("Pause", BuiltInActions::Pause);
            RegisterAction("UISelect", BuiltInActions::UISelect);
            RegisterAction("UICancel", BuiltInActions::UICancel);
            RegisterAction("UINavigate", BuiltInActions::UINavigate);
            RegisterAction("Idle", BuiltInActions::Idle);
            RegisterAction("MoveLeft", BuiltInActions::MoveLeft);
            RegisterAction("MoveRight", BuiltInActions::MoveRight);
            RegisterAction("MoveUp", BuiltInActions::MoveUp);
            RegisterAction("MoveDown", BuiltInActions::MoveDown);
        }
    private:
        ActionRegistry() { RegisterBuiltIns(); }
        uint32_t m_NextCustomID = BuiltInActions::BeginCustomRange;
    };
}

// Specialize std::hash for ActionType to allow it to be used in unordered_map and unordered_set
namespace std {
    template <>
    struct hash<Spoon::ActionType> {
        std::size_t operator()(const Spoon::ActionType& action) const noexcept {
            return std::hash<uint32_t>()(action.m_ID);
        }
    };
}
