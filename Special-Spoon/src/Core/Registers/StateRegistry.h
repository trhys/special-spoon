#pragma once

#include "Core/Core.h"
#include <unordered_map>
#include <vector>
#include <stdexcept>

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
        constexpr uint32_t None = 0;
        constexpr uint32_t Quit = 1;

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

        void RegisterState(const std::string& name, uint32_t id)
        {
            m_NameToID[name] = id;
            m_IDToName[id] = name;
        }

        void RegisterBuiltIns()
        {
            RegisterState("None", BuiltInStates::None);
            RegisterState("Quit", BuiltInStates::Quit);
        }

        void RegisterCustom(const std::string& name)
        {
            RegisterState(name, m_NextCustomID++);
        }

        StateType GetState(const std::string& name)
        {
            if (m_NameToID.find(name) != m_NameToID.end())
            {
                return StateType{ m_NameToID[name] };
            }
            return StateType{ BuiltInStates::None };
        }

        StateType GetFromID(const uint32_t& id)
        {
          if (m_IDToName.find(id) != m_IDToName.end())
          {
            return StateType{ id };
          }
          throw std::runtime_error("attempted to fetch non existent state type from id");
        }

        std::string& GetName(const StateType& action)
        {
            return m_IDToName[action.m_ID];
        }

        std::unordered_map<std::string, uint32_t> m_NameToID;
        std::unordered_map<uint32_t, std::string> m_IDToName;
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

