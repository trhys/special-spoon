#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
	struct RenderLayer : public ComponentBase<RenderLayer>
	{
		RenderLayer(int layer) : ComponentBase::ComponentBase("RenderLayer"), m_Layer(layer) {}

		int m_Layer;

		void OnReflect() override
		{
			
		}
	};
}