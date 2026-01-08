#pragma once

#include "Core.h"
#include "ActionQueue.h"
#include "ProjectManager.h"
#include "SceneManager.h"
#include "EntityManager/EntityManager.h"
#include "Renderer.h"

#include "System/InputSystem.h"
#include "System/SystemManager.h"

#include "Editor/Editor.h"

#include "SFML/Graphics.hpp"

namespace Spoon 
{ 
	struct AppSpecifications
	{
		sf::VideoMode mode = sf::VideoMode::getDesktopMode();
		std::string windowName = "Special Spoon";
		bool editorEnabled = true;
		// REFACTORING : Moving these to project manager
		// std::filesystem::path assetsDir = "assets/";
		// std::filesystem::path dataDir = "data/";
	};

	class SPOON_API Application
	{
	public:
		Application(const AppSpecifications& specs);
		virtual ~Application() {}

		void Close();
		void Run();
		void Update(sf::Time tick);
		void Shutdown();
		
		static Application& Get() { return *s_Instance; }

		ProjectManager& GetProjectManager() { return m_ProjectManager; }
		EntityManager& GetEntityManager() 	{ return m_EntityManager; }
		SceneManager& GetSceneManager()  	{ return m_SceneManager; }
		SystemManager& GetSystemManager() 	{ return m_SystemManager; }
		Renderer& GetRenderer()			  	{ return m_Renderer; }
		Editor& GetEditor()				  	{ return m_Editor; }
		Viewport& GetViewport()		  	  	{ return m_Viewport; }
		sf::RenderWindow& GetWindow()     	{ return m_Window; }
		ActionQueue& GetActionQueue()     	{ return m_ActionQueue; }

	private:
		static Application* s_Instance;
		bool m_IsRunning = true;
		bool closePrompt = false;
		
		ProjectManager	  m_ProjectManager;
		SceneManager	  m_SceneManager;
		EntityManager	  m_EntityManager;
		InputSystem		  m_InputSystem;
		SystemManager	  m_SystemManager;
		Renderer		  m_Renderer;
		Editor 			  m_Editor;
		ActionQueue	  	  m_ActionQueue;

		AppSpecifications m_Specs;
		Viewport		  m_Viewport;
		sf::RenderWindow  m_Window;
		
	};

	//DEFINE IN CLIENT APPLICATION
	//SET APPSPECIFICATIONS

	Application* CreateApp();
}