#pragma once

#include "Core/Project/Policies.h"
#include <nlohmann/json.hpp>

namespace Spoon {
	struct ProjectConfig {
		ActiveSortPolicy SortPolicy = ActiveSortPolicy::Isometric;
		nlohmann::json DefaultEnabledSystems = nlohmann::json::array();
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectConfig, SortPolicy, DefaultEnabledSystems)
}
