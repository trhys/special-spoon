#pragma once

namespace Spoon {
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
