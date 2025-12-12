#pragma once

#include "ECS/Components/Component.h"

namespace Spoon
{
	struct RenderLayer : public Component
	{
		RenderLayer(int layer) : m_Layer(layer) {}

		int m_Layer;
	};
}