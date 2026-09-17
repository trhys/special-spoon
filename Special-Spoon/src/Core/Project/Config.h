#pragma once

#include "Core/Project/Policies.h"

namespace Spoon {
	struct ProjectConfig {
		bool PhysicsEnabled;
		std::unique_ptr<SortPolicy> SortPolicy;
	}
}
