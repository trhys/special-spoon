#pragma once
#include <string>
#include <cstdint>

namespace Spoon
{
	void ErrorMessage(const std::string& message);
	void AvailableActionsList(uint32_t& selectedAction, const std::string& listBoxID);
}