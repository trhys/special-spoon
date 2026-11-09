#pragma once

#include "Core/Core.h"

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace Spoon
{
    struct StateType
    {
        uint32_t m_ID = 0;
        bool operator==(const StateType& other) const { return m_ID == other.m_ID; }
        bool operator!=(const StateType& other) const { return m_ID != other.m_ID; }
        explicit operator bool() const { return m_ID != 0; }
    };

    namespace BuiltInStates
    {
		// Base states for the system
        constexpr uint32_t None = 0;
        constexpr uint32_t Quit = 1;
		constexpr uint32_t Idle = 2;      // idle state for movement

		// Movement states for 8-way movement
		constexpr uint32_t MoveUp = 3;      // move up state for movement
		constexpr uint32_t MoveDown = 4;    // move down state for movement
		constexpr uint32_t MoveLeft = 5;    // move left state for movement
		constexpr uint32_t MoveRight = 6;   // move right state for movement
		constexpr uint32_t MoveUR = 7;      // move up-right state for movement
		constexpr uint32_t MoveUL = 8;      // move up-left state for movement
		constexpr uint32_t MoveDR = 9;      // move down-right state for movement
		constexpr uint32_t MoveDL = 10;      // move down-left state for movement

		// Idle states for 8-way movement
		constexpr uint32_t IdleUp = 11;    // idle up state for movement
		constexpr uint32_t IdleDown = 12;  // idle down state for movement
		constexpr uint32_t IdleLeft = 13;  // idle left state for movement
		constexpr uint32_t IdleRight = 14; // idle right state for movement
		constexpr uint32_t IdleUR = 15;     // idle up-right state for movement
		constexpr uint32_t IdleUL = 16;     // idle up-left state for movement
		constexpr uint32_t IdleDR = 17;     // idle down-right state for movement
		constexpr uint32_t IdleDL = 18;     // idle down-left state for movement

		// Custom states can be registered starting from this ID
        constexpr uint32_t BeginCustomRange = 1000;
    }

    class SPOON_API StateRegistry
    {
    public:
        static StateRegistry& Get()
        {
            static StateRegistry instance;
            return instance;
        }

        void RegisterState(const std::string& name, uint32_t id);
        void RegisterBuiltIns();
        void RegisterCustom(const std::string& name);

        StateType GetState(const std::string& name);
        StateType GetFromID(const uint32_t& id);
        std::string& GetName(const StateType& action);

        void BuiltSorted();
        const std::vector<std::string>& GetSortedNames();

    public:
        std::unordered_map<std::string, uint32_t> m_NameToID;
        std::unordered_map<uint32_t, std::string> m_IDToName;
		std::vector<std::string> m_SortedNames;

    private:
        StateRegistry() { RegisterBuiltIns(); }
        uint32_t m_NextCustomID = BuiltInStates::BeginCustomRange;
    };
}

namespace std {
    template <>
    struct hash<Spoon::StateType> {
        std::size_t operator()(const Spoon::StateType& action) const noexcept {
            return std::hash<uint32_t>()(action.m_ID);
        }
    };
}

