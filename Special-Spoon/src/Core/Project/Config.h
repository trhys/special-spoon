#pragma once

#include "Core/Project/Policies.h"

namespace Spoon {
	struct ProjectConfig {
		bool PhysicsEnabled = false;
		ActiveSortPolicy SortPolicy = 0;
	}
}
