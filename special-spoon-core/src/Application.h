#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "ResourceManager.h"
#include "Physics/PhysicsManager.h"
#include "Scene/SceneManager.h"

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

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);		
		void UpdatePhysics();
		void Close();
		void Run();

		void CreateScene(std::string name, sf::Vector2f size);
		
		AppSpecifications GetSpecs() { return m_Specs; }
		SceneManager* GetSM() { return &m_SceneManager; }
		ResourceManager* GetRM() { return &m_ResourceManager; }

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