#pragma once

#include "Core/Project/Policies.h"
#include <nlohmann/json.hpp>

namespace Spoon {
	struct ProjectConfig {
		bool PhysicsEnabled = false;
		ActiveSortPolicy SortPolicy = ActiveSortPolicy::Isometric;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectConfig, PhysicsEnabled, SortPolicy)
}
