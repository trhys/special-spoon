#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "Physics/PhysicsManager.h"
#include "ResourceManager.h"
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
		void ProcessLayerQueue();	
		void UpdatePhysics();
		void Close();
		void Run();
		
		static Application& Get() { return *s_Instance; }
		AppSpecifications GetSpecs() { return m_Specs; }
		sf::RenderWindow& GetWindow() { return m_Window; }

	private:
		static Application* s_Instance;
		bool m_IsRunning = true;

		AppSpecifications m_Specs;
		sf::RenderWindow  m_Window;
		LayerStack 		  m_LayerStack;
		ResourceManager   m_ResourceManager;
		PhysicsManager    m_PhysicsManager;
		SceneManager	  m_SceneManager;

		std::vector<Layer*> m_LayerQueue;
	};

	//DEFINE IN CLIENT APPLICATION
	//SET APPSPECIFICATIONS

	Application* CreateApp();
}