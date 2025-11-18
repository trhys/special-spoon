#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "Physics/PhysicsManager.h"
#include "SFML/Graphics.hpp"

namespace Spoon 
{ 

	class SceneManager;
	class ResourceManager;
	class PhysicsManager;

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

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);		
		void UpdatePhysics();
		void Close();
		void Run();
		
		AppSpecifications GetSpecs() { return m_Specs; }
		SceneManager* GetSM();
		ResourceManager* GetRM();

	private:
		static Application* s_Instance;
		bool m_IsRunning = true;

		AppSpecifications m_Specs;
		sf::RenderWindow  m_Window;
		LayerStack 		  m_LayerStack;
		ResourceManager   m_ResourceManager;
		PhysicsManager    m_PhysicsManager;
		SceneManager	  m_SceneManager;
	};

	//DEFINE IN CLIENT APPLICATION
	//SET APPSPECIFICATIONS

	Application* CreateApp();
}