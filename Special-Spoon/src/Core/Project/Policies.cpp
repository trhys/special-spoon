#include "Core/Project/Policies.h"

namespace Spoon {
	void IsometricProjection::ComputeDepth(std::vector<Renderable>& renderables)
	{
		for (auto& renderable : renderables)
			{
				renderable.m_Depth = renderable.m_Component->GetPosition().y;
			}
	}

	const char* SortPolicyToString(ActiveSortPolicy policy)
	{
		switch (policy) {
		case (ActiveSortPolicy::Isometric):
			return "Isometric";
		default:
			return "None";
		}
	}
}
