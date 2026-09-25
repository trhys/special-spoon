#pragma once

#include "Core/Renderer/Renderable.h"
#include <vector>
#include <nlohmann/json.hpp>

namespace Spoon {
	// enum class pointing to sort policy
	enum class ActiveSortPolicy {
		Isometric
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(ActiveSortPolicy, {
		{ActiveSortPolicy::Isometric, "isometric"}
	})

	// helper for editor
	const char* SortPolicyToString(ActiveSortPolicy policy);

	// interface for depth sorting policies
	class SortPolicy {
		public:
			virtual ~SortPolicy() = default;
			virtual void ComputeDepth(std::vector<Renderable>& renderables) = 0;
	};

	class IsometricProjection : public SortPolicy {
		public:
			void ComputeDepth(std::vector<Renderable>& renderables) override;
	};
}
