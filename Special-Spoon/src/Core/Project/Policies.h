#pragma once

#include "Core/Renderer/Renderable.h"
#include <vector>
#include <nlohmann/json.hpp>

namespace Spoon {
	// enum class pointing to sort policy
	enum class ActiveSortPolicy {
		Isometric,
		TopDown
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(ActiveSortPolicy, {
		{ActiveSortPolicy::Isometric, "isometric"},
		{ActiveSortPolicy::TopDown, "topdown"}
	})

	// helper for editor
	const char* SortPolicyToString(ActiveSortPolicy policy);

	// projection gate - whether logical positions are isometrically
	// projected for presentation (see Core/Renderer/WorldProjection.h)
	inline bool UsesIsometricProjection(ActiveSortPolicy policy)
	{
		return policy == ActiveSortPolicy::Isometric;
	}

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

	class TopDownProjection : public SortPolicy {
		public:
			void ComputeDepth(std::vector<Renderable>& renderables) override;
	};
}
