#include "Core/Project/Policies.h"

namespace Spoon {
	void IsometricProjection::ComputeDepth(std::vector<Renderable>& renderables)
	{
		for (auto& renderable : renderables)
			{
				sf::Vector2f pos = renderable.m_Component->GetPosition();
				float x = pos.x;
				float y = pos.y;
				renderable.m_Depth = x + y;
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
