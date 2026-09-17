#pragma once

namespace Spoon {
// enum class pointing to sort policy
	enum class ActiveSortPolicy {
		Isometric
	}

// interface for depth sorting policies
	class SortPolicy {
		public:
			virtual void ComputeDepth(std::vector<Renderable>& renderables) = 0;
	};

	class IsometricProjection : public SortPolicy {
		public:
			void ComputeDepth(std::vector<Renderable>& renderables) override;
	};
}
