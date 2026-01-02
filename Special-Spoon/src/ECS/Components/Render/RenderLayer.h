#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
	struct RenderLayer : public ComponentBase<RenderLayer>
	{
		RenderLayer(int layer = 0) : ComponentBase::ComponentBase("RenderLayer"), m_Layer(layer) {}

		int m_Layer;

		void OnReflect() override
		{
			ImGui::InputInt("Layer", &m_Layer);
		}
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RenderLayer, m_Layer)
}