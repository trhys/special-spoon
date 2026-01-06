#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
	struct RenderLayer : public ComponentBase<RenderLayer>
	{
		RenderLayer(int layer = 0) : ComponentBase::ComponentBase(Name), m_Layer(layer) {}

		static constexpr const char* Name = "RenderLayer";
		int m_Layer;

		void OnReflect() override
		{
			ImGui::InputInt("Layer", &m_Layer);
		}
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RenderLayer, m_Layer)
}