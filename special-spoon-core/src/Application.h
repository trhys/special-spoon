#pragma once

#include "Core.h"
#include "Layer.h"
#include <vector>

namespace Spoon { 

	class SPOON_API Application
	{
	public:
		Application();
		virtual ~Application() {}

		// LAYER CONTROL
		Layer* CreateLayer();
		void PushLayer(Layer* layer);

		std::vector<Layer*> GetLayerStack();

		void Run();

	private:
		bool m_IsRunning = true;

		std::vector<Layer*> LayerStack;

	};

	//APPLICATION ENTRY POINT ---- DEFINE IN CLIENT APPLICATION
	Application* CreateApp();

}