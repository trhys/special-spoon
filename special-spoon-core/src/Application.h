#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "ResourceManager.h"

#include "SFML/Graphics.hpp"

namespace Spoon { 

	struct AppSpecifications
	{
		sf::Vector2u m_WindowSize = {600, 600};
		std::string m_WindowName = "Special Spoon";
	};

	class SPOON_API Application
	{
	public:
		Application(const AppSpecifications& specs);//, std::unordered_map<std::string, std::filesystem::path> resources);
		virtual ~Application() {}

		// CONTROLS RENDERWINDOW SETTINGS
		AppSpecifications GetSpecs() { return m_Specs; }
		
		// LAYER/SCENE CONTROL
		void PushLayer(Layer* layer);
		void LoadScenes();

		// GAME LOOP CONTROL
		void Run();
		void Close();

	private:
		static Application* s_Instance;

		AppSpecifications m_Specs;
		sf::RenderWindow m_Window;

		bool m_IsRunning = true;

		LayerStack m_LayerStack;
		ResourceManager m_RSM;
	};

	//APPLICATION ENTRY POINT ---- DEFINE IN CLIENT APPLICATION
	//SHOULD SET WINDOW SIZE AND WINDOW NAME IN APPSPECIFICATIONS

	Application* CreateApp();
}