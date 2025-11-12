#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "ResourceManager.h"
#include "Physics/PhysicsManager.h"
#include "SFML/Graphics.hpp"

namespace Spoon { 

	struct AppSpecifications
	{
		sf::Vector2u m_WindowSize = {600, 600};
		std::string m_WindowName = "Special Spoon";
		bool PhysicsEnabled = false;
	};

	class SPOON_API Application
	{
	public:
		Application(const AppSpecifications& specs);
		virtual ~Application() {}

		AppSpecifications GetSpecs() { return m_Specs; }

		void PushLayer(Layer* layer);
		void UpdatePhysics();
		void Run();
		void Close();

	private:
		static Application* s_Instance;
		bool m_IsRunning = true;

		AppSpecifications m_Specs;
		sf::RenderWindow  m_Window;
		LayerStack 		  m_LayerStack;
		ResourceManager   m_RSM;
		PhysicsManager    m_PM;
	};

	//DEFINE IN CLIENT APPLICATION
	//SET APPSPECIFICATIONS

	Application* CreateApp();
}