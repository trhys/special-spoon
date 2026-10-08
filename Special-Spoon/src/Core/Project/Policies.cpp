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

	void TopDownProjection::ComputeDepth(std::vector<Renderable>& renderables)
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
		case (ActiveSortPolicy::TopDown):
			return "Top-Down";
		default:
			return "None";
		}
	}
}
