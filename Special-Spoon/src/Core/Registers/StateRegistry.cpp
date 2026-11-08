#include "StateRegistry.h"
#include <algorithm>

namespace Spoon
{
	void StateRegistry::RegisterBuiltIns()
	{
		RegisterState("None", BuiltInStates::None);
		RegisterState("Quit", BuiltInStates::Quit);
		RegisterState("Idle", BuiltInStates::Idle);
		RegisterState("MoveUp", BuiltInStates::MoveUp);
		RegisterState("MoveDown", BuiltInStates::MoveDown);
		RegisterState("MoveLeft", BuiltInStates::MoveLeft);
		RegisterState("MoveRight", BuiltInStates::MoveRight);
		RegisterState("MoveUR", BuiltInStates::MoveUR);
		RegisterState("MoveUL", BuiltInStates::MoveUL);
		RegisterState("MoveDR", BuiltInStates::MoveDR);
		RegisterState("MoveDL", BuiltInStates::MoveDL);
		RegisterState("IdleUp", BuiltInStates::IdleUp);
		RegisterState("IdleDown", BuiltInStates::IdleDown);
		RegisterState("IdleLeft", BuiltInStates::IdleLeft);
		RegisterState("IdleRight", BuiltInStates::IdleRight);
		RegisterState("IdleUR", BuiltInStates::IdleUR);
		RegisterState("IdleUL", BuiltInStates::IdleUL);
		RegisterState("IdleDR", BuiltInStates::IdleDR);
		RegisterState("IdleDL", BuiltInStates::IdleDL);
		BuiltSorted();
	}

	void StateRegistry::RegisterState(const std::string& name, uint32_t id)
	{
		m_NameToID[name] = id;
		m_IDToName[id] = name;
	}

	void StateRegistry::RegisterCustom(const std::string& name)
	{
		RegisterState(name, m_NextCustomID++);
		BuiltSorted();
	}

	StateType StateRegistry::GetState(const std::string& name)
	{
		if (m_NameToID.find(name) != m_NameToID.end())
		{
			return StateType{ m_NameToID[name] };
		}
		return StateType{ BuiltInStates::None };
	}

	StateType StateRegistry::GetFromID(const uint32_t& id)
	{
		if (m_IDToName.find(id) != m_IDToName.end())
		{
			return StateType{ id };
		}
		throw std::runtime_error("attempted to fetch non existent state type from id");
	}

	std::string& StateRegistry::GetName(const StateType& action)
	{
		return m_IDToName[action.m_ID];
	}

	void StateRegistry::BuiltSorted()
	{
		m_SortedNames.clear();
		for (const auto& [name, id] : m_NameToID)
		{
			m_SortedNames.push_back(name);
		}
		std::sort(m_SortedNames.begin(), m_SortedNames.end());
	}

	const std::vector<std::string>& StateRegistry::GetSortedNames()
	{
		return m_SortedNames;
	}
}