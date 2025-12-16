#pragma once

#include "Core.h"

#include "ResourceManager.h"
#include "SceneManager.h"
#include "EntityManager.h"
#include "System/InputSystem.h"
#include "System/SystemManager.h"

#include "Editor/Editor.h"

#include "SFML/Graphics.hpp"

namespace Spoon 
{ 
	struct AppSpecifications
	{
		sf::Vector2u m_WindowSize = {600, 600};
		std::string m_WindowName = "Special Spoon";
		bool m_EditorEnabled = true;
	};

	class SPOON_API Application
	{
	public:
		Application(const AppSpecifications& specs);
		virtual ~Application() {}

		void Close();
		void Run();
		
		static Application& Get() { return *s_Instance; }
		AppSpecifications GetSpecs() { return m_Specs; }
		
		SceneManager& GetSceneManager() { return m_SceneManager; }
		EntityManager& GetEntityManager() { return m_EntityManager; }
		ResourceManager& GetResourceManager() { return m_ResourceManager; }
		InputSystem& GetInputSystem() { return m_InputSystem; }
		SystemManager& GetSystemManager() { return m_SystemManager; }

	private:
		static Application* s_Instance;
		bool m_IsRunning = true;	
		
		ResourceManager   m_ResourceManager;
		SceneManager	  m_SceneManager;
		EntityManager	  m_EntityManager;
		InputSystem		  m_InputSystem;
		SystemManager	  m_SystemManager;
		Editor 			  m_Editor;

		AppSpecifications m_Specs;
		sf::RenderWindow  m_Window;
	};

	//DEFINE IN CLIENT APPLICATION
	//SET APPSPECIFICATIONS

	Application* CreateApp();
}