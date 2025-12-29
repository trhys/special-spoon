#pragma once

#include "Core.h"
#include "SceneManager.h"
#include "EntityManager.h"
#include "System/InputSystem.h"
#include "System/SystemManager.h"
#include "Renderer.h"
#include "Editor/Editor.h"

#include "SFML/Graphics.hpp"

namespace Spoon 
{ 
	struct AppSpecifications
	{
		sf::VideoMode mode = sf::VideoMode::getDesktopMode();
		std::string windowName = "Special Spoon";
		bool editorEnabled = true;
		std::filesystem::path assetsDir = "assets/";
		std::filesystem::path dataDir = "data/";
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
		void HandleEditorGizmos();
		
		static Application& Get() { return *s_Instance; }

		EntityManager& GetEntityManager() { return m_EntityManager; }

	private:
		static Application* s_Instance;
		bool m_IsRunning = true;
		bool closePrompt = false;
		
		SceneManager	  m_SceneManager;
		EntityManager	  m_EntityManager;
		InputSystem		  m_InputSystem;
		SystemManager	  m_SystemManager;
		Renderer		  m_Renderer;
		Editor 			  m_Editor;

		AppSpecifications m_Specs;
		sf::View 		  m_Camera;
		sf::RenderTexture m_EditorViewport;
		sf::RenderWindow  m_Window;
		
	};

	//DEFINE IN CLIENT APPLICATION
	//SET APPSPECIFICATIONS

	Application* CreateApp();
}