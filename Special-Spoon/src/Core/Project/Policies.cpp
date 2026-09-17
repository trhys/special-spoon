#include "Core/Projects/Policies.h"

namespace Spoon {
	void IsometricProjection::ComputeDepth(std::vector<Renderable>& renderables)
	{
		for (auto& renderable : renderables)
			{
				renderable.m_Depth = renderable.m_Component->GetPosition().y;
			}
	}
}
